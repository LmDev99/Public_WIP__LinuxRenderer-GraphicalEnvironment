/*
 * Copyright (c) 2026 Logan Mottern
 * All rights reserved.
 *
 * This source code is provided for portfolio and review purposes only.
 * No permission is granted to copy, modify, distribute, sublicense, or use
 * this software except with the express written permission of the copyright owner.
 */

#pragma once
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>




typedef struct{

}exrPixelData;

struct exrHeaderData{

    bool magicNumberFound;
    bool bit9;
    bool bit10;
    bool bit11;
    bool bit12;


    int displayWindowUpperLeftPixelCoordinateX;
    int displayWindowUpperLeftPixelCoordinateY;
    int displayWindowBottomRightPixelCoordinateX;
    int displayWindowBottomRightPixelCoordinateY;

    int dataWindowUpperLeftPixelCoordinateX;
    int dataWindowUpperLeftPixelCoordinateY;
    int dataWindowBottomRightPixelCoordinateX;
    int dataWindowBottomRightPixelCoordinateY;

    float aspectRatio;

    int lineOrder;
    int compression;

    bool channelsAreSupported;
    

    
};


//
int getStringSize(char* string){
    int i = 0;
    while(string[i] != '\0'){
        i = i + 1;
    }
    return i + 1; //add one since we start count from 0

}

bool isStringEqual(const char* stringA, const char* stringB,int* returnStringASize){
    int i = 0;
    for(i; stringA[i] == stringB[i]; i++){
        if((stringA[i+1] == '\0') && (stringB[i+1] == '\0')){
            *returnStringASize = i + 2; //add one since we start count from 0, and another since we exit before i is iterated on
            return true;
        }


    }

    while(stringA[i] != '\0'){
        i = i + 1;
    }
    *returnStringASize = i + 1; //add one since we start count from 0
    return false;
    
}


int GET_FILE_SIZE(int fileDescriptor){

    struct stat fileStats;
    fstat(fileDescriptor,&fileStats);
    return fileStats.st_size;

}

void printRawDataBytes(int size, char* rawData){
    //printf("[");
    for(int i = 0; i < size; i++){
        //printf("%d(%c) ", rawData[i], rawData[i]);
        if(i%10 == 0){
            //printf("\n");
        }
    }
    //printf("]");
}

void readExrFile(const char* fileName){
    int fileDescriptor = open(fileName,O_RDONLY);
    if(fileDescriptor == -1){return;}
    int fileSize = GET_FILE_SIZE(fileDescriptor);
    char* rawDataBuffer = (char*)(malloc(fileSize));
    if(read(fileDescriptor,rawDataBuffer,fileSize) != fileSize){
        return;
    }
 

    printRawDataBytes(fileSize / 4,rawDataBuffer);

    free(rawDataBuffer);
}








void parseChannelListAttribute(char* rawDataBuffer,int* currentByteIndex,exrHeaderData* exrHeader){
   //parse attribute type
    int advanceIndex = 0;
    if(isStringEqual(rawDataBuffer+(*currentByteIndex),"chlist",&advanceIndex) != 1){
        return;
    }
    *currentByteIndex = *currentByteIndex + advanceIndex;

    int attributeSize = *((int*)(rawDataBuffer + *currentByteIndex));
    
    *currentByteIndex = *currentByteIndex + 4;


    char* channelName0 = "A";
    char* channelName1 = "B";
    char* channelName2 = "G";
    char* channelName3 = "R";
    char* channelNames[4] = {channelName0,channelName1,channelName2,channelName3};

    int pixelTypes[4] = {-1,-1,-1,-1}; 
    int pLinears[4] = {-1,-1,-1,-1};
    int xSampling[4] = {-1,-1,-1,-1};
    int ySampling[4] = {-1,-1,-1,-1};

    bool parsingAttributeValue = 1;

    while(parsingAttributeValue){
        bool matchFound = 0;
        if(rawDataBuffer[(*currentByteIndex)] == '\0'){
            *currentByteIndex = *currentByteIndex + 1;
            parsingAttributeValue = 0;
            break;
        }

        int i = 0;
        for(i; i < 4; i++){
            if(isStringEqual(rawDataBuffer + (*currentByteIndex),channelNames[i],&advanceIndex)){
                matchFound = 1;
                break;
            }
        }

        if(matchFound){
            *currentByteIndex = *currentByteIndex + advanceIndex;     

            pixelTypes[i] = *((int*)(rawDataBuffer+(*currentByteIndex)));
            *currentByteIndex = *currentByteIndex + 4;

            pLinears[i] = rawDataBuffer[(*currentByteIndex)];
            *currentByteIndex = *currentByteIndex + 4;

            xSampling[i] = *((int*)(rawDataBuffer+(*currentByteIndex)));
            *currentByteIndex = *currentByteIndex + 4;
            ySampling[i] = *((int*)(rawDataBuffer+(*currentByteIndex)));
            *currentByteIndex = *currentByteIndex + 4;
        }
        else{
            exrHeader->channelsAreSupported = 0;
            return;

        }
    }

    for(int j = 0; j < 4; j++){
        if((pixelTypes[j] != 2) || (pLinears[j] != 0) || (xSampling[j] != 1) || (ySampling[j] != 1)){}
        //printf("Channel = %d : pixelType = %d : pLinear = %d : xSampling = %d : ySampling = %d \n",j,pixelTypes[j],pLinears[j],xSampling[j],ySampling[j]);
    }

    


}


void parseDisplayWindow(char* rawDataBuffer,int* currentByteIndex,exrHeaderData* exrHeader){
    //parse attribute type
    int advanceIndex = 0;
    if(isStringEqual(rawDataBuffer+(*currentByteIndex),"box2i",&advanceIndex) != 1){
       
        return;
    }
    *currentByteIndex = *currentByteIndex + advanceIndex;

    int attributeSize = *((int*)(rawDataBuffer + *currentByteIndex));

    *currentByteIndex = *currentByteIndex + 4;


    exrHeader->displayWindowUpperLeftPixelCoordinateX = 0;
    exrHeader->displayWindowUpperLeftPixelCoordinateY = 0;
    exrHeader->displayWindowBottomRightPixelCoordinateX = 0;
    exrHeader->displayWindowBottomRightPixelCoordinateY = 0;
    

    exrHeader->displayWindowUpperLeftPixelCoordinateX = *((int*)(rawDataBuffer + *currentByteIndex));

    *currentByteIndex = *currentByteIndex + 4;

    exrHeader->displayWindowUpperLeftPixelCoordinateY = *((int*)(rawDataBuffer + *currentByteIndex));

    *currentByteIndex = *currentByteIndex + 4;

    exrHeader->displayWindowBottomRightPixelCoordinateX = *((int*)(rawDataBuffer + *currentByteIndex));

    *currentByteIndex = *currentByteIndex + 4;

    exrHeader->displayWindowBottomRightPixelCoordinateY = *((int*)(rawDataBuffer + *currentByteIndex));
  
    *currentByteIndex = *currentByteIndex + 4;

  
    
}

void parseDataWindow(char* rawDataBuffer,int* currentByteIndex,exrHeaderData* exrHeader){
    //parse attribute type
    int advanceIndex = 0;
    if(isStringEqual(rawDataBuffer+(*currentByteIndex),"box2i",&advanceIndex) != 1){
    
        return;
    }
    *currentByteIndex = *currentByteIndex + advanceIndex;

    int attributeSize = *((int*)(rawDataBuffer + *currentByteIndex));
    //printf("parseDataWindow() : attributeSize = %d \n", attributeSize);
    *currentByteIndex = *currentByteIndex + 4;


   


    exrHeader->dataWindowUpperLeftPixelCoordinateX = *((int*)(rawDataBuffer + *currentByteIndex));

    *currentByteIndex = *currentByteIndex + 4;

    exrHeader->dataWindowUpperLeftPixelCoordinateY = *((int*)(rawDataBuffer + *currentByteIndex));
   
    *currentByteIndex = *currentByteIndex + 4;

    exrHeader->dataWindowBottomRightPixelCoordinateX = *((int*)(rawDataBuffer + *currentByteIndex));
   
    *currentByteIndex = *currentByteIndex + 4;

    exrHeader->dataWindowBottomRightPixelCoordinateY = *((int*)(rawDataBuffer + *currentByteIndex));
  
    *currentByteIndex = *currentByteIndex + 4;

  
    
}


void parsePixelAspectRatio(char* rawDataBuffer, int* currentByteIndex,exrHeaderData* exrHeader){
      //parse attribute type
    int advanceIndex = 0;
    if(isStringEqual(rawDataBuffer+(*currentByteIndex),"float",&advanceIndex) != 1){
        
        return;
    }
    *currentByteIndex = *currentByteIndex + advanceIndex;

    int attributeSize = *((int*)(rawDataBuffer + *currentByteIndex));
 
    *currentByteIndex = *currentByteIndex + 4;


    
    exrHeader->aspectRatio = *((float*)(rawDataBuffer + *currentByteIndex));;
    
    *currentByteIndex = *currentByteIndex + 4;

  

}

void parseLineOrder(char* rawDataBuffer, int* currentByteIndex,exrHeaderData* exrHeader){
    //parse attribute type
    int advanceIndex = 0;
    if(isStringEqual(rawDataBuffer+(*currentByteIndex),"lineOrder",&advanceIndex) != 1){
        return;
    }
    *currentByteIndex = *currentByteIndex + advanceIndex;

    int attributeSize = *((int*)(rawDataBuffer + *currentByteIndex));
    
    *currentByteIndex = *currentByteIndex + 4;

    
    exrHeader->lineOrder = rawDataBuffer[(*currentByteIndex)];
    
    *currentByteIndex = *currentByteIndex + 1;

  

}



void parseCompression(char* rawDataBuffer, int* currentByteIndex,exrHeaderData* exrHeader){
    int advanceIndex = 0;
    if(isStringEqual(rawDataBuffer+(*currentByteIndex),"compression",&advanceIndex) != 1){
       
        return;
    }
    *currentByteIndex = *currentByteIndex + advanceIndex;

    int attributeSize = *((int*)(rawDataBuffer + *currentByteIndex));
   
    *currentByteIndex = *currentByteIndex + 4;

    
    exrHeader->compression = rawDataBuffer[(*currentByteIndex)];
    
    *currentByteIndex = *currentByteIndex + 1;

  



}



void todo(char* rawDataBuffer, int* currentByteIndex,exrHeaderData* exrHeader){
    int advanceIndex = getStringSize(rawDataBuffer+(*currentByteIndex));
  
    *currentByteIndex = *currentByteIndex + advanceIndex;
    

    int currentAttributeSize = *((int*)(rawDataBuffer+(*currentByteIndex)));


    *currentByteIndex = *currentByteIndex + 4 + currentAttributeSize;

}





void parseOffsetTable(char* rawDataBuffer, int* currentByteIndex){}







float* readExrFile3(const char* fileName,exrHeaderData* exrHeader, float* returnScanlinesRGBA){
    int fileDescriptor = open(fileName,O_RDONLY);
    if(fileDescriptor == -1){return nullptr;}
    
    int fileSize = GET_FILE_SIZE(fileDescriptor);
 
    char* rawDataBuffer = (char*)(malloc(fileSize));
    if(read(fileDescriptor,rawDataBuffer,fileSize) != fileSize){

        return nullptr;
    }
    

    int currentByteIndex = 0;

    if( (*(int*)(rawDataBuffer + currentByteIndex)) == 20000630){
        exrHeader->magicNumberFound = 1;
     
    }

    currentByteIndex = currentByteIndex + 4;

    currentByteIndex = currentByteIndex + 1;
    
    exrHeader->bit9 = ((rawDataBuffer[currentByteIndex] & (1 << 1)) != 0);
    exrHeader->bit10 = ((rawDataBuffer[currentByteIndex] & (1 << 2)) != 0);
    exrHeader->bit11 = ((rawDataBuffer[currentByteIndex] & (1 << 3)) != 0);
    exrHeader->bit12 = ((rawDataBuffer[currentByteIndex] & (1 << 4)) != 0);



    currentByteIndex = currentByteIndex + 3;



    ///////////////////////////////Read Header/////////////////////////////////////////////////////////////////////////////////////////

    char* attributeName0 = "displayWindow";
    char* attributeName1 = "dataWindow";
    char* attributeName2 = "pixelAspectRatio";
    char* attributeName3 = "screenWindowCenter";
    char* attributeName4 = "screenWindowWidth";
    char* attributeName5 = "lineOrder";
    char* attributeName6 = "compression";
    char* attributeName7 = "channels";
    char* attributeNames[8] = {attributeName0,attributeName1,attributeName2,attributeName3,
                              attributeName4,attributeName5,attributeName6,attributeName7};

    void (*parsingFunctions[8])(char*, int*,exrHeaderData*);
    parsingFunctions[0] = parseDisplayWindow;
    parsingFunctions[1] = parseDataWindow;
    parsingFunctions[2] = parsePixelAspectRatio;
    parsingFunctions[3] = todo;
    parsingFunctions[4] = todo;
    parsingFunctions[5] = parseLineOrder;
    parsingFunctions[6] = parseCompression;
    parsingFunctions[7] = parseChannelListAttribute;



    int advanceIndex = 0;
    bool matchFound = 0;


    for(int readingHeader = 0; readingHeader < 100; readingHeader++){
        matchFound = 0;
        int i = 0;
        for(i = 0; i < 8; i++){
            if(isStringEqual(rawDataBuffer+currentByteIndex,attributeNames[i],&advanceIndex)){
                matchFound = 1;
                break;
                
            }
        }

        if(matchFound){
            if(rawDataBuffer[currentByteIndex] == 0){break;}
            currentByteIndex = currentByteIndex + advanceIndex;
            parsingFunctions[i](rawDataBuffer,&currentByteIndex,exrHeader);

       

        }


        else{
            if(rawDataBuffer[currentByteIndex] == 0){break;}

            currentByteIndex = currentByteIndex + advanceIndex;

            advanceIndex = getStringSize(rawDataBuffer+currentByteIndex);

            currentByteIndex = currentByteIndex + advanceIndex;


            int currentAttributeSize = *((int*)(rawDataBuffer+currentByteIndex));



            currentByteIndex = currentByteIndex + 4 + currentAttributeSize;

        }

    }

    int numberOfScanLines = exrHeader->dataWindowBottomRightPixelCoordinateY - exrHeader->dataWindowUpperLeftPixelCoordinateY + 1;
  



    ///////////////////////Read Pixels/////////////////////////////////////////////////////////////////////////////////////

    currentByteIndex = currentByteIndex + 1;

    int currentOffsetTableByteIndex = currentByteIndex;
    for(int k = 0; k < (exrHeader->dataWindowBottomRightPixelCoordinateY + 1);k++){
        u_int64_t currentScanlineOffsetPosition = *((u_int64_t*)(rawDataBuffer + currentOffsetTableByteIndex));

        
        currentByteIndex = currentScanlineOffsetPosition;


        int yScanLine = *((int*)(rawDataBuffer + currentByteIndex));

        currentByteIndex = currentByteIndex + 4;

        int pixelByteSize = *((int*)(rawDataBuffer + currentByteIndex));

        currentByteIndex = currentByteIndex + 4;

        int scanLineChannelStride = pixelByteSize / 4;



        



    
        for(int i = 0; i < (exrHeader->dataWindowBottomRightPixelCoordinateX + 1); i++){
            for(int j = 3; j >= 0; j--){
                    memcpy((returnScanlinesRGBA + ((exrHeader->dataWindowBottomRightPixelCoordinateX + 1)*yScanLine*4) + (i*4) + 3 - j),(float*)(rawDataBuffer+currentByteIndex + (scanLineChannelStride * j) + (i*4)),sizeof(float));
                 
                }
                

        }

        currentOffsetTableByteIndex = currentOffsetTableByteIndex + 8;




    
    }

    return returnScanlinesRGBA;



    

}