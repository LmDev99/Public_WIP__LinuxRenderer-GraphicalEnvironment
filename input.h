/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * LinuxRenderer-GraphicalEnvironment - custom Linux/Vulkan rendering and UI engine
 * Copyright (C) 2026 Logan Mottern
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */


#pragma once
#include <unistd.h>
#include <poll.h> 
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <signal.h>
//#include "../cameraData.hpp"



#define STARTTIME_UNINITIALIZED -9999


void printByte(char x){
    for(int i=0, bitIndex = 7; bitIndex > -1; i++,bitIndex--){
        bool currentBitValue = (x & (1 << bitIndex)) != 0;
        printf("%d",currentBitValue);
    }
    printf("\n");

}

void printLongLong(unsigned long long x){
    for(int i=0, bitIndex = 63; bitIndex > -1; i++,bitIndex--){
        bool currentBitValue = (x & (1ULL << bitIndex)) != 0;
        printf("%d",currentBitValue);
    }
    printf("\n");
}

//This function takes in two strings and returns 1 if the two strings are identical
// and returns 0 if the two strings are not identical
bool isStringEqual(const char* stringA, const char* stringB){
    int i = 0;
    for(i; stringA[i] == stringB[i]; i++){
        if((stringA[i+1] == '\0') && (stringB[i+1] == '\0')){
            return true;
        }

    }

    return false;
    
} 

char* copyString(const char* inputString){
    //first get size
    int i = 0;
    for(i; inputString[i] != '\0'; i++){

    }
    char* returnString = (char*)malloc(i+1);
    i = 0;
    for(i; inputString[i] != '\0'; i++){
        returnString[i] = inputString[i];
    }
    returnString[i] = '\0';

    return returnString;
}

//This function takes in two strings: stringA and stringB
//the function returns 1 if stringA is a substring of stringB
//and returns 0 if the StringA is not a substring of stringB
int isSubstring(const char* stringA, const char* stringB){
    int i = 0;
    int isStringCurrentlyMatching = 0;
    for(i; stringB[i] != '\0'; i++){
       if((stringA[i] == stringB[i]) && (isStringCurrentlyMatching != -1)){
           isStringCurrentlyMatching = 1;
       }
       if(stringA[i] != stringB[i]){
           isStringCurrentlyMatching = -1;
       }
       if((isStringCurrentlyMatching == 1) && (stringA[i+1] == '\0')){
           return 1;
       }

    }

    return 0;
    
}

//This function takes in two strings: stringA and stringB
//stringA represents a directory path and stringB represents a directory name
//the function concatenates the two strings and leaves a forward slash at the end
//to create a directory path
char* createNewDirectoryPath(const char* stringA, const char* stringB){
    int stringASize = 0;
    while(stringA[stringASize] != '\0'){
        stringASize = stringASize + 1;
    }

    int stringBSize = 0;
    while(stringB[stringBSize] != '\0'){
        stringBSize = stringBSize + 1;
    }



    char* returnString = (char*)malloc((stringASize+stringBSize+2) * sizeof(char));
    memcpy(returnString, stringA, stringASize * sizeof(char));
    memcpy(returnString+stringASize, stringB, stringBSize * sizeof(char));
    returnString[stringASize+stringBSize] = '/';
    returnString[stringASize+stringBSize+1] = '\0';
    


    return returnString;

}




//This function takes in two strings: stringA and stringB
//stringA represents a directory path and stringB represents a file name
//the function concatenates the two strings to create a file path
char* createNewFilePath(const char* stringA, const char* stringB){
    int stringASize = 0;
    while(stringA[stringASize] != '\0'){
        stringASize = stringASize + 1;
    }

    int stringBSize = 0;
    while(stringB[stringBSize] != '\0'){
        stringBSize = stringBSize + 1;
    }



    char* returnString = (char*)malloc((stringASize+stringBSize+1) * sizeof(char));
    memcpy(returnString, stringA, stringASize * sizeof(char));
    memcpy(returnString+stringASize, stringB, stringBSize * sizeof(char));
    returnString[stringASize+stringBSize] = '\0';
    return returnString;

}










//A "byte" is 8 bits, and a "nibble" is 4 bits. Thus a byte contains two nibbles
//this function reverse the orders of the bits within each nibble
//NOTE: The order of the nibbles within the byte themselves do NOT change order
//Example: 0111 0100  --->  1110 0010

// 



void reverseNibbleBitsInByte(char* inputByte){
    //since we are going to be bit shifting and we need to retain a copy of the orginal byte
    char copyOfInputByte = *inputByte;
    bool bits[8] = {0};
    for(int i = 0; i < 8;i++){
        //we mask the bit 0x80 since this is the most signicant bit of a byte
        if(copyOfInputByte & 0x80){
            bits[i] = 1;
        }
        copyOfInputByte = copyOfInputByte << 1;
    }
/*
    printf("printing byte copy \n");
    for(int i = 0; i < 8; i++){
        printf("%d,",bits[i]);
    }
    printf("\n");
*/
    //currentNibbleStride brings us from the last bitIndex of the first nibble to the last bitIndex of the second nibble
    int currentNibbleStride = 0;
    int currentMask = 0x80;
    for(int i = 0; i < 8; i++){
        if(i==4){
            currentNibbleStride = 8;
        }
        if(*inputByte & currentMask){
            if(bits[3+currentNibbleStride-i]){
                //since the bit is 1 in the current placement of the nibble and is also 1 in the reversed placement of the nibble
                //no action is needed
            }
            else{
                //since the bit is 1 in the current placement of the nibble but is 0 in the reversed placement of the nibble
                //we need to set the current bit in the nibble From 1 To 0
                *inputByte = *inputByte & ~currentMask;

            }
        }
        else{
            if(bits[3+currentNibbleStride-i]){
                //since the bit is 0 in the current placment of the nibble but is 1 in the reversed placement of the nibble
                //we need to set the current bit in the nibble From 0 To 1
                *inputByte = *inputByte | currentMask;


            }
            else{
                //since the bit is 0 in the current placement of the nibble and is also 0 in the reversed placement of the nibble
                //no action is needed

            }

        }

        currentMask = currentMask >> 1;

        //printf("printing byte AFTER i = %d \n", i);
        //printByte(*inputByte);

    }

}


















struct eventFileCapabilities{
    const char* inputEventFileName;
    bool xRel;
    bool yRel;

    //Mouse Left Button, Mouse Right Button, Mouse Middle Button
    bool supportedMouseKeys[3];
    //Starting at 0 is the alphanumericals in A,B,C,...,1,2,3,...
    bool supportedKeyboard[41];

    void init(){
        xRel = 0;
        yRel = 0;
        supportedMouseKeys[0] = 0;
        supportedMouseKeys[1] = 0;
        supportedMouseKeys[2] = 0;
        memset(supportedKeyboard,0,41);
    
        
    }

    bool keyboardAllKeySupported(){
        for(int i = 0; i < 41; i++){
            if(supportedKeyboard[i] != 1){
                return 0;
            }
        }
        return 1;

    }
};


struct inputDevices{
    int numberOfMouseDevices;
    const char** mouseInputEventFileNames;

    int numberOfKeyboardDevices;
    const char** keyboardInputEventFileNames;
};







struct mainThread_ControllerState{
    int keyboardKeysCurrentlyPressed[255]; //0 is not held down, 1 is the first , 2 is held down
    double keyboardKeysCurrentlyPressed_DeltaTiming[255]; //this is the last deltaTime the Main Thread consumed from Input Thread

     // Modifier state as interpreted from published records
    bool shiftHeld;
    bool ctrlHeld;
    bool altHeld;
    bool capsLockToggled;
};



struct controllerState{ //this will be renamed to inputThread_ControllerState
    bool mouseKeysCurrentlyPressed[3];
    double mouseKeysCurrentlyPressed_StartTiming[3];
    double mousedKeysCurrentlyPressed_DeltaTiming[3];


    bool keyboardKeysCurrentlyPressed[255];
    double keyboardKeysCurrentlyPressed_StartTiming[255];
    double keyboardKeysCurrentlyPressed_DeltaTiming[255];
    int currentRelX = 0;
    int currentRelY = 0;

    bool isControllerActive(){
        if(currentRelX || currentRelY || mouseKeysCurrentlyPressed[0] || mouseKeysCurrentlyPressed[1] || mouseKeysCurrentlyPressed[2]){
            return 1;

        }
        for(int i = 0; i < 255; i++){
            if(keyboardKeysCurrentlyPressed[i]){
                return 1;

            }
        }

        return 0;

    }

    void updateKeyTimings(double currentTime){
        for(int i = 0; i < 255; i++){
            if(keyboardKeysCurrentlyPressed[i]){
                keyboardKeysCurrentlyPressed_DeltaTiming[i] = currentTime - keyboardKeysCurrentlyPressed_StartTiming[i];
            }
            else if(keyboardKeysCurrentlyPressed[i] == 0 && keyboardKeysCurrentlyPressed_DeltaTiming[i] > 0){
                keyboardKeysCurrentlyPressed_DeltaTiming[i] = -1 * keyboardKeysCurrentlyPressed_DeltaTiming[i];
            }
            else if(keyboardKeysCurrentlyPressed[i] == 0 && keyboardKeysCurrentlyPressed_DeltaTiming[i] < 0){
                keyboardKeysCurrentlyPressed_DeltaTiming[i] = 0;
            }
        }

    }

    

    

};
struct KeyPressRecord{
    //this is needed for inputSystem to main thread transfer verification, will be removed once input system is finished
    //The first two KeyPressRecords are always the header first and then the second keyPressRecord uses the first int to describe how many keyPressRecords occured in this pass
    int paddingForInputHeader;
    int keyCode;
    double startTime;
    double deltaTime;
};
struct RecordedKeyPresses{

    //int numberOfTimesKeyPressedInThisPass[255];
    int controllerScanResult[255];
    int lastKeyPressRecordEntryIndice[255];

    int RecordedKeyPressesBufferSize;
    int NumberOfRecordedEntriesInPass ;
    KeyPressRecord* RecordedKeyPresses;
    



    

    int initializeRecordedKeyPresses(int recordedKeyPressesSize, bool reinitialize = 0){
        if(recordedKeyPressesSize <= 3){
            return -1; //return error since we need atleast 3 KeyPressRecords for header,numberOfEntries,and footnote
        }
        RecordedKeyPressesBufferSize = recordedKeyPressesSize;
        if(reinitialize == 0){RecordedKeyPresses = (KeyPressRecord*)malloc(sizeof(KeyPressRecord) * recordedKeyPressesSize);}
        for(int i =0; i < 255; i++){
            lastKeyPressRecordEntryIndice[i] = -1;

        }
        
        //memset(numberOfTimesKeyPressedInThisPass,0, sizeof(int) * 255);
        memset(controllerScanResult,0, sizeof(int) * 255);
        NumberOfRecordedEntriesInPass = 0;
        return 0;


    }

    int addEntry(int code, double startTime, double endTime){
        if(NumberOfRecordedEntriesInPass >= (RecordedKeyPressesBufferSize - 4) ){ //since we need to reserve atleast 3 spaces for header,numberOfEntries,and footnote
            return -1;
        }
        KeyPressRecord newEntry; newEntry.keyCode = code; newEntry.startTime = startTime; newEntry.deltaTime = endTime;
        RecordedKeyPresses[NumberOfRecordedEntriesInPass] = newEntry;
        lastKeyPressRecordEntryIndice[code] = NumberOfRecordedEntriesInPass;
        NumberOfRecordedEntriesInPass = NumberOfRecordedEntriesInPass + 1;


    }

    

    void scanController(controllerState* controller, double currentTime){
        for(int i = 0; i < 255; i++){
            if(controller->keyboardKeysCurrentlyPressed[i] == 1){
                controllerScanResult[i] = (controllerScanResult[i] >= 1) + 1;
                if(controllerScanResult[i] == 1){
                    addEntry(i,controller->keyboardKeysCurrentlyPressed_StartTiming[i],0);
                }
                else if(controllerScanResult[i] == 2){
                    int entryIndex = lastKeyPressRecordEntryIndice[i];
                    RecordedKeyPresses[entryIndex].deltaTime = controller->keyboardKeysCurrentlyPressed_DeltaTiming[i];
                }
                

            }
            if(controller->keyboardKeysCurrentlyPressed_DeltaTiming[i] <0){
                int entryIndex = lastKeyPressRecordEntryIndice[i];
                RecordedKeyPresses[entryIndex].deltaTime = -controller->keyboardKeysCurrentlyPressed_DeltaTiming[i]; //switch back to Positive
                controllerScanResult[i] = 0;
            }
        }
    }   
};

/*
struct inputThreadToUiData{
    int mouseX[4];
    int mouseY[4];
    char inputTextBuffer[500][4];
    
};
*/

void printControllerState(controllerState* controller){
    for(int i = 0; i < 3; i++){
        printf("%d,",controller->mouseKeysCurrentlyPressed[i]);

    }
    for(int i = 0; i < 255; i++){
        printf("%d,",controller->keyboardKeysCurrentlyPressed[i]);

    }

    printf("\n");

    printf("currentRelX = %d \n",controller->currentRelX);
    printf("currentRelY = %d \n",controller->currentRelY);

}






int keyboardKeyNumberToASCII(int keyNumber){

    // -1 -> KEY_RESERVED       -4 ->       -7 ->
// -2 ->ESC                 -5 ->       -8 ->
// -3 ->                    -6 ->       -9 ->
int keyNumberToAsciiTable[69] = {-1,-2,                         //1
                                49,50,51,52,53,54,55,56,57,48,  //11
                                45,61,                          //13
                                -3, -4,                         //15
                                81,87,69,82,84,89,85,73,79,80,  //25
                                91,93,                           //27
                                -5,-6,                          //29
                                65,83,68,70,71,72,74,75,76,     //38
                                59,39,96,                       //41
                                -7,                             //42
                                92,                             //43
                                90,88,67,86,66,78,77,           //50
                                44,46,47,                       //53
                                -8,-9,-10,                      //56
                                32,                             //57
                                -11,                            //58
                                -12,-13,-14,-15,-16,-17,-18,
                                -19,-20,21                      //68




                                };

    return keyNumberToAsciiTable[keyNumber];
    

}







void parseRelFile(eventFileCapabilities* capabilities, unsigned long long hexStringValue){
        unsigned long long currentMask = 1ULL << 63;
        if (hexStringValue & currentMask) {
            capabilities->xRel = 1;
            //printf("CAP REL X \n");
        } 
        else {
            capabilities->xRel = 0;
        }
        
        currentMask = currentMask >> 1;
        if (hexStringValue & currentMask) {
            capabilities->yRel = 1;
            //printf("CAP REL Y\n");
        } 
        else {
            capabilities->yRel = 0;
        }

}


void parseKeyFile(eventFileCapabilities* capabilities, unsigned long long* hexStringValues, int hexStringValuesSize){
    unsigned char keyCodes[41] = {KEY_A,KEY_B,KEY_C,KEY_D,KEY_E,KEY_F,KEY_G,KEY_H,KEY_I,KEY_J,KEY_K,KEY_L,KEY_M,KEY_N,KEY_O,KEY_P,KEY_Q,KEY_R,KEY_S,KEY_T,KEY_U,KEY_V,KEY_W,KEY_X,KEY_Y,KEY_Z,
                                   KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9,KEY_0,
                                   KEY_MINUS,KEY_EQUAL,KEY_BACKSPACE,KEY_ENTER,
                                   KEY_ESC};

    for(int keyIndex = 0; keyIndex < 41; keyIndex++){
        unsigned long long currentMask = 1ULL << 63;
        int keyCode = keyCodes[keyIndex];
        int hexStringValuesIndex = keyCode / 64;
        int bitPosition = keyCode % 64;

        if(hexStringValuesIndex < hexStringValuesSize){
            if(hexStringValues[hexStringValuesIndex] & (currentMask >> (bitPosition))){
                //printf("ATLEAST THIS IS HAPPENING keyIndex = %d \n",keyCodes[keyIndex]);
                //printf("hexStringValuesIndex = %d, bitPosition = %d \n",hexStringValuesIndex,bitPosition);
                capabilities[0].supportedKeyboard[keyIndex] = 1;
            }
            else{
                //printf("THIS SHOULDNT HAPPEN keyIndex = %d \n",keyCodes[keyIndex]);
                //printf("hexStringValuesIndex = %d, bitPosition = %d \n",hexStringValuesIndex,bitPosition);
                capabilities[0].supportedKeyboard[keyIndex] = 0;

            }

        }
    }


    int mouseKeyCodes[3] = {BTN_LEFT,BTN_RIGHT,BTN_MIDDLE};
    for(int keyIndex = 0; keyIndex < 3; keyIndex++){
        unsigned long long currentMask = 1ULL << 63;
        int keyCode = mouseKeyCodes[keyIndex];
        int hexStringValuesIndex = keyCode / 64;
        int bitPosition = keyCode % 64;

        if(hexStringValuesIndex < hexStringValuesSize){
            if(hexStringValues[hexStringValuesIndex] & (currentMask >> (bitPosition))){
                //printf("MOUSE SUPPORTED KEY: keyIndex = %d \n",mouseKeyCodes[keyIndex]);
                //printf("hexStringValuesIndex = %d, bitPosition = %d \n",hexStringValuesIndex,bitPosition);
                capabilities[0].supportedMouseKeys[keyIndex] = 1;
               
            }
            else{
                //printf("MOUSE NOT SUOOPRTED KEY keyIndex = %d \n",mouseKeyCodes[keyIndex]);
                //printf("hexStringValuesIndex = %d, bitPosition = %d \n",hexStringValuesIndex,bitPosition);
                capabilities[0].supportedMouseKeys[keyIndex] = 0;
              

            }

        }

    }
    
    
}

//stringLength does NOT include null terminator
void parseHexStringIntoLong_64(char* hexString,int stringLength,unsigned long long* returnLong,int* returnLongBufferLastIndex){

    //printf("DEBUGGING parseHexStringIntoLong_64() \n");
    int i = stringLength;
    //printf("DEBUGGING parseHexStringIntoLong_64() : i : %d \n",i);
    int j = 0;
    int currentHexGroupingLength = 0;
    //printf("DEBUGGING parseHexStringIntoLong_64() : ENTERING WHILE LOOP \n");
    while(i >= 0){
        //printf("DEBUGGING parseHexStringIntoLong_64() : While Loop : i : %d \n",i);
        bool characterWasValid = 0;
        char currentByte = hexString[i];
        //printf("DEBUGGING parseHexStringIntoLong_64() : While Loop : currentByte = %d \n",currentByte);
        if (currentByte >= '0' && currentByte <= '9'){
            //printf("DEBUGGING parseHexStringIntoLong_64() : While Loop : currentByte falls in between '0' and '9' \n");
            returnLong[j] += currentByte - '0';
            //printf("DEBUGGING parseHexStringIntoLong_64() : While Loop : returnLong[%d] : print :",j);
            //printLongLong(returnLong[j]);
            currentHexGroupingLength = currentHexGroupingLength + 1;
            //printf("DEBUGGING parseHexStringIntoLong_64() : While Loop : currentHexGroupingLength = %d \n",currentHexGroupingLength);
            characterWasValid = 1;
            
        }
        else if (currentByte >= 'a' && currentByte <= 'f'){
            //printf("DEBUGGING parseHexStringIntoLong_64() : While Loop : currentByte falls in between 'a' and 'f' \n");
            returnLong[j] += 10 + (currentByte - 'a');
            //printLongLong(returnLong[j]);
            currentHexGroupingLength = currentHexGroupingLength + 1;
            //printf("DEBUGGING parseHexStringIntoLong_64() : While Loop : currentHexGroupingLength = %d \n",currentHexGroupingLength);
            characterWasValid = 1;
            
        }
        else if (currentByte >= 'A' && currentByte <= 'F'){
            //printf("DEBUGGING parseHexStringIntoLong_64() : While Loop : currentByte falls in between 'A' and 'F' \n");
            returnLong[j] += 10 + (currentByte - 'A');
            //printLongLong(returnLong[j]);
            currentHexGroupingLength = currentHexGroupingLength + 1;
            //printf("DEBUGGING parseHexStringIntoLong_64() : While Loop : currentHexGroupingLength = %d \n",currentHexGroupingLength);
            characterWasValid = 1;
            
        }

        //if the currentByte is a space character we move to next unsigned long long
        else if(hexString[i] == 32){
            //printf("DEBUGGING parseHexStringIntoLong_64() : While Loop : currentByte is a space character\n");
            //pad remainbytes to current unsigned long long before incrementing j
            //printf("BEFORE SHIFT :\n");
            //printLongLong(returnLong[j]);
            int shiftAmount = (16 - currentHexGroupingLength) * 4;// Calculate the number of bits to shift to left-pad the number
            //printf("final shitft amount = %d \n",shiftAmount);
            returnLong[j] <<= shiftAmount; // Shift the entire result to the left to pad with zeros on the right
            //printLongLong(returnLong[j]);
            j = j + 1;
            currentHexGroupingLength = 0;
        }

       
        i = i - 1;
        if(characterWasValid){
            //printf("character was valid!\n");
            if(i >=0){
                if(currentHexGroupingLength < 16 && hexString[i] != 32){
                    //printf("character was valid and i >= 0, this means we bit shift 4 spaces \n");
                    returnLong[j] <<= 4;
                }

            }
            //the else branch is only taken at the very end to ensure padding is recieved on final hex group
            else{
                //printf("this was the final hexgroup\n");
                int shiftAmount = (16 - currentHexGroupingLength) * 4;// Calculate the number of bits to shift to left-pad the number
                //printf("final shitft amount = %d \n",shiftAmount);
                returnLong[j] <<= shiftAmount;

            }
        }

    }

    *returnLongBufferLastIndex = j;



    
}





void parseCapabilities(char* directoryPath,const char* inputFolderName,eventFileCapabilities* eventCapabilities){
    
    DIR* capabilitiesDirectoryData = opendir(directoryPath);
    dirent* directoryEntry;
    //eventFileCapabilities eventCapabilities = {};
    //eventCapabilities[0].inputEventFileName = inputFolderName;
    eventCapabilities[0].init();
    do{
        directoryEntry = readdir(capabilitiesDirectoryData);
        if(directoryEntry){
            if(isStringEqual(directoryEntry->d_name,"rel")){
                printf("DEBUGGING parseCapabilities() : %s \n",directoryEntry->d_name);
                char* relFilePath  = createNewFilePath(directoryPath,"rel");
                int fileDescriptor = open(relFilePath,O_RDONLY);
                free(relFilePath);
                char memoryBuffer[32] = {0};
                int bytesRead = read(fileDescriptor,memoryBuffer,32);

                
                //print raw memory buffer
                //printf("parseCapabilities : print raw memory buffer\n");
                //for(int i = 0; i < bytesRead; i++){
                //    printf("parseCapabilities : memoryBuffer[%d] = %d,    ",i,memoryBuffer[i]);
                //}
                //printf("bytesRead = %d \n",bytesRead);
                
                
                int hexStringValuesBufferSize = 200;
                int hexStringValuesLastIndex = 0;
                unsigned long long hexStringValues[hexStringValuesBufferSize];
                memset(hexStringValues,0,200*8);
                parseHexStringIntoLong_64(memoryBuffer,bytesRead-1,hexStringValues,&hexStringValuesLastIndex);
                //for(int i = 0; i < hexStringValuesLastIndex+1;i++){
                //    printf("DEBUGGING parseCapabilities() :rel: HEX STRING %lu \n",hexStringValues[i]);
                //}
                for(int i = 0; i < hexStringValuesLastIndex+1; i++){
                    unsigned long long* currentHexStringValuePointer = hexStringValues + i;
                    char* bytePointer = ((char*)currentHexStringValuePointer);
                    char* bytesWithinHexStringValue[8] = {bytePointer,bytePointer+1,bytePointer+2,bytePointer+3,bytePointer+4,
                                                        bytePointer+5,bytePointer+6,bytePointer+7};
                    for(int j = 7; j > -1; j--){
                        //printf("Byte[%d] in hexStringValues[%d] :Before Reversal : \n",j,i);
                        //printByte(*bytesWithinHexStringValue[j]);
                        reverseNibbleBitsInByte(bytesWithinHexStringValue[j]);
                        //printf("Byte[%d] in hexStringValues[%d] : After Reversal : \n",j,i);
                        //printByte(*bytesWithinHexStringValue[j]);
                    }
                    //printf("\n\n\n\n");
                }


                printLongLong(hexStringValues[0]);
                parseRelFile(eventCapabilities,hexStringValues[0]);
                //if(eventCapabilities.xRel == 1 && eventCapabilities.yRel == 1){
                    //printf("FOUND THE MOUSE!\n");
                //}
              
            }
            
            else if(isStringEqual(directoryEntry->d_name,"key")){

                
                
                printf("DEBUGGING parseCapabilities() : %s \n",directoryEntry->d_name);
                char* keyFilePath  = createNewFilePath(directoryPath,"key");
                int fileDescriptor = open(keyFilePath,O_RDONLY);
                free(keyFilePath);
                char memoryBuffer[100] = {0};
                int bytesRead = read(fileDescriptor,memoryBuffer,100);
                //printf("bytesRead = %d \n",bytesRead);
                //printf("reading hexstring directly from memoryBuffer: %s",memoryBuffer);
                
            



                int hexStringValuesBufferSize = 200;
                int hexStringValuesLastIndex = 0;
                unsigned long long hexStringValues[hexStringValuesBufferSize];
                memset(hexStringValues,0,200*8);
                parseHexStringIntoLong_64(memoryBuffer,bytesRead-1,hexStringValues,&hexStringValuesLastIndex);
                for(int i = 0; i < hexStringValuesLastIndex+1; i++){
                    unsigned long long* currentHexStringValuePointer = hexStringValues + i;
                    char* bytePointer = ((char*)currentHexStringValuePointer);
                    char* bytesWithinHexStringValue[8] = {bytePointer,bytePointer+1,bytePointer+2,bytePointer+3,bytePointer+4,
                                                        bytePointer+5,bytePointer+6,bytePointer+7};
                    for(int j = 7; j > -1; j--){
                        //printf("Byte[%d] in hexStringValues[%d] :Before Reversal : \n",j,i);
                        //printByte(*bytesWithinHexStringValue[j]);
                        reverseNibbleBitsInByte(bytesWithinHexStringValue[j]);
                        //printf("Byte[%d] in hexStringValues[%d] : After Reversal : \n",j,i);
                        //printByte(*bytesWithinHexStringValue[j]);
                    }
                    
                }
                

                //for(int i = 0; i < hexStringValuesLastIndex+1; i++){
                //    printf("DEBUGGING parseCapabilities() :key: HEX STRING %lu \n",hexStringValues[i]);
                //}
    

                for(int i = 0; i < hexStringValuesLastIndex+1; i++){
                    printLongLong(hexStringValues[i]);
                }


                parseKeyFile(eventCapabilities,hexStringValues,hexStringValuesLastIndex+1);
                //if(eventCapabilities.keyboardAllKeySupported()){
                    //printf("FOUND THE KEYBOARD");
                //}

                
                

            }
            
            
        }
    }
    while(directoryEntry != 0);

    if(eventCapabilities[0].xRel == 1 && eventCapabilities[0].yRel == 1 && eventCapabilities[0].supportedMouseKeys[0] == 1 && eventCapabilities[0].supportedMouseKeys[1] == 1){
        printf("Input Folder : %s : is a MOUSE EVENT FILE \n",inputFolderName);

    }
    if(eventCapabilities[0].keyboardAllKeySupported()){
        printf("Input Folder : %s : is a KEYBOARD EVENT FILE \n",inputFolderName);

    }
    


}


void parseInputSubFolder(char* directoryPath,const char* inputFolderName,eventFileCapabilities* eventCapabilities){
    DIR* inputDirectoryData = opendir(directoryPath);
    dirent* directoryEntry;

    do{
        directoryEntry = readdir(inputDirectoryData);
        if(directoryEntry){
            if(isStringEqual(directoryEntry->d_name,"capabilities")){
                //printf("DEBUGGING parseInputSubFolder(): %s \n",directoryEntry->d_name);
                char* capabilitiesFolder = createNewDirectoryPath(directoryPath,directoryEntry->d_name);
                //printf("DEBUGGING parseInputSubFolder(): %s \n",capabilitiesFolder);
                parseCapabilities(capabilitiesFolder,inputFolderName,eventCapabilities);
                free(capabilitiesFolder);

            }
            else if(isSubstring("event",directoryEntry->d_name)){
                char* eventFileName = copyString(directoryEntry->d_name);
                eventCapabilities[0].inputEventFileName = eventFileName;

            }
        }

    }
    while(directoryEntry != 0);
    closedir(inputDirectoryData);


}

void identifyInputEventFiles(inputDevices* returnDetectedDevices){
    const char* systemInputDirectory = "/sys/class/input";
    DIR* systemInputDirectoryData = opendir(systemInputDirectory);
    
    dirent* directoryEntry;
    int numberOfInputFilesFound = 0;

    do{
        directoryEntry = readdir(systemInputDirectoryData);
        if(directoryEntry){
                if(isSubstring("input",directoryEntry->d_name)){
                    //printf("\n\n\nDEBUGGING identifyMouseEventFiles() : %s \n",directoryEntry->d_name);
                    numberOfInputFilesFound = numberOfInputFilesFound + 1;
                    //char* inputFolder = createNewDirectoryPath("/sys/class/input/",directoryEntry->d_name);
                    //printf("DEBUGGING identifyMouseEventFiles() : %s \n",inputFolder);
                    //parseInputSubFolder(inputFolder,directoryEntry->d_name);
                    //free(inputFolder);

                }
        }
    }
    while(directoryEntry != 0);




    rewinddir(systemInputDirectoryData);
    int directoryIndex = 0;
    eventFileCapabilities eventCapabilities[numberOfInputFilesFound]; 
    do{
        bool detectedInputDevice = 0;
        directoryEntry = readdir(systemInputDirectoryData);
        if(directoryEntry){
                if(isSubstring("input",directoryEntry->d_name)){
                    printf("\n\n\nDEBUGGING identifyMouseEventFiles() : %s \n",directoryEntry->d_name);
                    char* inputFolder = createNewDirectoryPath("/sys/class/input/",directoryEntry->d_name);
                    printf("DEBUGGING identifyMouseEventFiles() : %s \n",inputFolder);
                    parseInputSubFolder(inputFolder,directoryEntry->d_name,eventCapabilities + directoryIndex);
                    free(inputFolder);
                   
                    if(eventCapabilities[directoryIndex].keyboardAllKeySupported() || (eventCapabilities[directoryIndex].xRel == 1 && eventCapabilities[directoryIndex].yRel == 1 && eventCapabilities[directoryIndex].supportedMouseKeys[0] && eventCapabilities[directoryIndex].supportedMouseKeys[1])){
                        detectedInputDevice = 1;
                    }
                    directoryIndex = directoryIndex + 1;
                }
        }
    }
    while(directoryEntry != 0);


    int mouseEventFileCount = 0;
    int mouseEventFileIndices[numberOfInputFilesFound];
    int keyboardEventFileCount = 0;
    int keyboardEventFileIndices[numberOfInputFilesFound];
    for(int i = 0; i < numberOfInputFilesFound; i++){
        if(eventCapabilities[i].keyboardAllKeySupported()){
            printf("identifyEventFiles() : eventCapabilities : %s : is a KEYBOARD EVENT FILE \n",eventCapabilities[i].inputEventFileName);
            keyboardEventFileIndices[keyboardEventFileCount] = i;
            keyboardEventFileCount = keyboardEventFileCount + 1;
        }
        else if(eventCapabilities[i].xRel == 1 && eventCapabilities[i].yRel == 1 && eventCapabilities[i].supportedMouseKeys[0] == 1 && eventCapabilities[i].supportedMouseKeys[1] == 1){
            printf("identifyEventFiles() : eventCapabilities : %s : is a MOUSE EVENT FILE \n",eventCapabilities[i].inputEventFileName);
            mouseEventFileIndices[mouseEventFileCount] = i;
            mouseEventFileCount = mouseEventFileCount + 1;
        }
        else{
            printf("identifyEventFiles() : eventCapabilities : %s : is NOT A RECOGNIZED EVENT FILE \n",eventCapabilities[i].inputEventFileName);
        }
    }

    printf("HITTING HERE!!!!!!! \n");
    const char** mouseEventFileNames = (const char**)malloc(mouseEventFileCount * sizeof(char*));
    for(int i = 0; i < mouseEventFileCount; i++){
        const char* mouseEventFileName = eventCapabilities[mouseEventFileIndices[i]].inputEventFileName;
        mouseEventFileNames[i] = mouseEventFileName;
    }

    const char** keyboardEventFileNames = (const char**)malloc(keyboardEventFileCount * sizeof(char*));
    for(int i = 0; i < keyboardEventFileCount; i++){
        const char* keyboardEventFileName = eventCapabilities[keyboardEventFileIndices[i]].inputEventFileName;
        keyboardEventFileNames[i] = keyboardEventFileName;
    } 


    (*returnDetectedDevices).numberOfMouseDevices = mouseEventFileCount;
    (*returnDetectedDevices).mouseInputEventFileNames = mouseEventFileNames;

    (*returnDetectedDevices).numberOfKeyboardDevices = keyboardEventFileCount;
    (*returnDetectedDevices).keyboardInputEventFileNames = keyboardEventFileNames;

    closedir(systemInputDirectoryData);

    printf("PRINT : %d \n",numberOfInputFilesFound);

}


void obtainInputDeviceFileDescriptors(inputDevices* detectedInputDevices,int* mouseEventFileDescriptors, int* keyboardEventFileDescriptors){
    const char* inputEventsDirectory = "/dev/input/";
    DIR* systemInputDirectoryData = opendir(inputEventsDirectory);

    dirent* directoryEntry;
    int numberOfInputFilesFound = 0;

    

    do{
        directoryEntry = readdir(systemInputDirectoryData);
        if(directoryEntry){
                for(int i = 0; i < detectedInputDevices->numberOfMouseDevices; i++){
                    if(isStringEqual(detectedInputDevices->mouseInputEventFileNames[i],directoryEntry->d_name)){
                        //printf("\n\n\nDEBUGGING identifyMouseEventFiles() : %s \n",directoryEntry->d_name);
                        char* mouseEventFileName = createNewFilePath(inputEventsDirectory,detectedInputDevices->mouseInputEventFileNames[i]);
                        printf("inputMain() : mouseEventFileName:  %s \n",mouseEventFileName);
                        mouseEventFileDescriptors[i] = open(mouseEventFileName,O_RDONLY);
                        if(mouseEventFileDescriptors[i] == -1){
                            printf("this is whats wrong \n");
                        }
                        free(mouseEventFileName);
                        //char* inputFolder = createNewDirectoryPath("/sys/class/input/",directoryEntry->d_name);
                        //printf("DEBUGGING identifyMouseEventFiles() : %s \n",inputFolder);
                        //parseInputSubFolder(inputFolder,directoryEntry->d_name);
                        //free(inputFolder);

                    }
                }
                for(int i = 0; i < detectedInputDevices->numberOfKeyboardDevices; i++){
                    if(isStringEqual(detectedInputDevices->keyboardInputEventFileNames[i],directoryEntry->d_name)){
                        //printf("\n\n\nDEBUGGING identifyMouseEventFiles() : %s \n",directoryEntry->d_name);
                        char* keyboardEventFileName = createNewFilePath(inputEventsDirectory,detectedInputDevices->keyboardInputEventFileNames[i]);
                        printf("inputMain() : keyboardEventFileName:  %s \n",keyboardEventFileName);
                        keyboardEventFileDescriptors[i] = open(keyboardEventFileName,O_RDONLY);
                        if(keyboardEventFileDescriptors[i] == -1){
                            printf("this is whats wrong \n");
                        }
                        free(keyboardEventFileName);
                        //char* inputFolder = createNewDirectoryPath("/sys/class/input/",directoryEntry->d_name);
                        //printf("DEBUGGING identifyMouseEventFiles() : %s \n",inputFolder);
                        //parseInputSubFolder(inputFolder,directoryEntry->d_name);
                        //free(inputFolder);

                    }
                }
        }
    }
    while(directoryEntry != 0);
    return;

}

void  createPollFileDescriptors(pollfd* fileDescriptors,int numberOfMouseEventFiles, int* mouseEventFileDescriptors,int numberOfKeyboardEventFiles, int* keyboardEventFileDescriptors){
    int i = 0;
    for(i;i < numberOfMouseEventFiles; i++){
        fileDescriptors[i].fd = mouseEventFileDescriptors[i];
        fileDescriptors[i].events = POLLIN;
        printf("rawInputV2 : i = %d fileDescriptors[%d].fd = %d \n",i,i,fileDescriptors[i].fd);

    }
    int j = 0;
    for(i,j; j < numberOfKeyboardEventFiles; i++,j++){
        fileDescriptors[i].fd = keyboardEventFileDescriptors[j];
        fileDescriptors[i].events = POLLIN;
        printf("rawInputV2 : i = %d : j = %d fileDescriptors[%d].fd = %d \n",i,j,i,fileDescriptors[i].fd);
    }

}





void rawInputV3(pollfd* fileDescriptors,int numberOfMouseEventFiles, int* mouseEventFileDescriptors,int numberOfKeyboardEventFiles, int* keyboardEventFileDescriptors,controllerState* controller, double currentTime){
  
   
    //printf("rawInputV2() about to poll \n");
    int ret = -1;
    if(controller->isControllerActive()){
        controller->updateKeyTimings(currentTime);
        ret = poll(fileDescriptors,numberOfMouseEventFiles+numberOfKeyboardEventFiles,0);

    }
    else{
        ret = poll(fileDescriptors,numberOfMouseEventFiles+numberOfKeyboardEventFiles,-1);
    }
    

    //printf("rawInputV2() finished poll \n");
    
    if(ret == -1){
        //printf("POLL ERROR!\n");
    }

    bool mouseMovedXAxis = 0;
    bool mouseMovedYAxis = 0;
    for(int i = 0; i < numberOfMouseEventFiles + numberOfKeyboardEventFiles; i++){
        if(fileDescriptors[i].revents & POLLIN){
            //read the input event from the read file descriptor
            input_event eventData[3] = {};
            int numberOfBytesRead = read(fileDescriptors[i].fd, eventData,sizeof(input_event) * 3);
            int numberOfInputEventsReceived = numberOfBytesRead / sizeof(input_event);
            if((numberOfBytesRead >= sizeof(input_event)) != 1){
                //printf("INCOMPLETE READ \n");
            }

            // Process the input event
            if (i < numberOfMouseEventFiles) {
                    //printf("Number Of Events Received = %d \n",numberOfInputEventsReceived);
                    for(int j = 0; j < numberOfInputEventsReceived; j++){
                        //printf("Mouse event: type=%d, code=%d, value=%d\n", eventData[j].type, eventData[j].code, eventData[j].value);
                        if(eventData[j].type == EV_REL && eventData[j].code == REL_X){
                            controller->currentRelX = eventData[j].value;
                            mouseMovedXAxis = 1;

                        }
                        else if(eventData[j].type == EV_REL && eventData[j].code == REL_Y){
                            controller->currentRelY = eventData[j].value;
                            mouseMovedYAxis = 1;

                        }
                        else if(eventData[j].type == EV_KEY){
                            if(eventData[j].value == 0 || eventData[j].value == 1){
                                controller->mouseKeysCurrentlyPressed[eventData[j].code - BTN_LEFT] = eventData[j].value;
                            }

                        }

                    }
            } 
            else if (i >= numberOfMouseEventFiles) {
                //printf("Number Of Events Received = %d \n",numberOfInputEventsReceived);
                for(int j = 0; j < numberOfInputEventsReceived; j++){
                    //printf("Keyboard event: type=%d, code=%d, value=%d\n", eventData[j].type, eventData[j].code, eventData[j].value);
                    if(eventData[j].type == EV_KEY){
                        //printf("This is a EV_KEY event : ASCII = %c \n",keyboardKeyNumberToASCII(eventData[j].code));
                        if(eventData[j].value == 2 || eventData[j].value == 1){
                            controller->keyboardKeysCurrentlyPressed[eventData[j].code] = 1;
                            if(controller->keyboardKeysCurrentlyPressed_StartTiming[eventData[j].code] == STARTTIME_UNINITIALIZED){
                                controller->keyboardKeysCurrentlyPressed_StartTiming[eventData[j].code] = currentTime;
                            }
                           


                        }
                        else if(eventData[j].value == 0){
                            controller->keyboardKeysCurrentlyPressed[eventData[j].code] = 0;
                            controller->keyboardKeysCurrentlyPressed_DeltaTiming[eventData[j].code] = -(currentTime - controller->keyboardKeysCurrentlyPressed_StartTiming[eventData[j].code]); //this is the finalized deltaTime, and is set to a negative to indicate that it is finalized
                            //should we reset startTiming to -9999 here?
                            
                        }

                    }
                    
                }

            }

        }
    }
    if(mouseMovedXAxis == 0){
        controller->currentRelX = 0;
    }
    if(mouseMovedYAxis == 0){
        controller->currentRelY = 0;
    }

    //printf("\n\n\n\n\n");
    
}






volatile int inputThreadRunning = 1;
void inputMain_ThreadSignalHandler(int signal_number){
    printf("Signal received, cleaning up and exiting...\n");
    __atomic_store_n(&inputThreadRunning, true, __ATOMIC_RELAXED);
    _exit(0); // Use _exit in a signal handler to exit immediately
}

int inputMain(void* args){
    
    signal(SIGUSR1,inputMain_ThreadSignalHandler);
    usleep(100000);
    inputDevices detectedInputDevices = {};
    identifyInputEventFiles(&detectedInputDevices);

    //const char* inputEventsDirectory = "/dev/input/";
    //DIR* systemInputDirectoryData = opendir(inputEventsDirectory);

    //dirent* directoryEntry;
    //int numberOfInputFilesFound = 0;
    

    int mouseEventFileDescriptors[detectedInputDevices.numberOfMouseDevices];
    int keyboardEventFileDescriptors[detectedInputDevices.numberOfKeyboardDevices];

    obtainInputDeviceFileDescriptors(&detectedInputDevices,mouseEventFileDescriptors,keyboardEventFileDescriptors);

    


    



    
    //printf("DEBUGGING inputMain() : numberOfMouseDevices = %d \n",detectedInputDevices.numberOfMouseDevices);
    for(int i = 0; i < detectedInputDevices.numberOfMouseDevices; i++){
        //printf("DEBUGGING inputMain() :detectedMouse[%d] = %s : \n",i,detectedInputDevices.mouseInputEventFileNames[i]);
    }

    //printf("DEBUGGING inputMain() : numberOfKeyboardDevices = %d \n",detectedInputDevices.numberOfKeyboardDevices);
    for(int i = 0; i < detectedInputDevices.numberOfKeyboardDevices; i++){
        //printf("DEBUGGING inputMain() : detectedKeyboard[%d] = %s : \n",i,detectedInputDevices.keyboardInputEventFileNames[i]);

    }

    //printf("DEBUGGING inputMain() : mouseEventFileDescriptors[0] = %d \n",mouseEventFileDescriptors[0]);
    //printf("DEBUGGING inputMain() : mouseEventFileDescriptors[1] = %d \n",mouseEventFileDescriptors[1]);
    //printf("DEBUGGING inputMain() : keyboardEventFileDescriptors[0] = %d \n",keyboardEventFileDescriptors[0]);

    pollfd pollFileDescriptors[detectedInputDevices.numberOfMouseDevices+detectedInputDevices.numberOfKeyboardDevices];
    createPollFileDescriptors(pollFileDescriptors,detectedInputDevices.numberOfMouseDevices,mouseEventFileDescriptors,detectedInputDevices.numberOfKeyboardDevices,keyboardEventFileDescriptors);
    
    controllerState controller = {};
    int running = 0;
    __atomic_load(&inputThreadRunning,&running,__ATOMIC_RELAXED);
    while(running){
        rawInputV3(pollFileDescriptors,detectedInputDevices.numberOfMouseDevices,mouseEventFileDescriptors,detectedInputDevices.numberOfKeyboardDevices,keyboardEventFileDescriptors,&controller,0);
        printControllerState(&controller);
        //printf("\n\n\n\n\n");
        __atomic_load((int*)args,&running,__ATOMIC_RELAXED);
    }

    return 0;

}




