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
#include <memory.h>

#define RETURN_SUCCESS 0
#define RETURN_FAILURE -1


typedef struct{
    int ID; 
    int pixelIndex;
    int width;
    int height; 
    int topLeftPixelCoordinateX;
    int topLeftPixelCoordinateY;
    int Layer;

}updateBlock;

class uiData;
class rowLineAlignment;
class rowLineRegion;












int returnMax(int a, int b){
    if(a < b){
        return b;
    }
    else{
        return a;
    }
}

int returnMin(int a, int b){
    if(a<b){
        return a;
    }
    else{
        return b;
    }
}

int getStringLength(char* string){
    int i = 0;
    for(i; string[i] != '\0'; i++){

    }
    return i;

}


int getGlyphWidth(char glyph){
    return 12;
}

int getGlyphHeight(char glpyh){
    return 12;
}


bool isPointInRectangle(int pointX, int pointY, int rectangleTopLeftX, int rectangleTopLeftY, int rectangleBottomRightX, int rectangleBottomRightY){
    return (pointX >= rectangleTopLeftX) && (pointX <= rectangleBottomRightX) && (pointY >= rectangleTopLeftY) && (pointY <= rectangleBottomRightY);
}



bool isRectangleInRectangle(int rectangle1TopLeftX, int rectangle1TopLeftY,int rectangle1BottomRightX, int rectangle1BottomRightY,
                            int rectangle2TopLeftX, int rectangle2TopLeftY,int rectangle2BottomRightX, int rectangle2BottomRightY,
                            updateBlock* returnOverlappingRegionUpdateBlock = 0, int overlapUpdateBlockID=0, int overlapUpdateBlockLayer = 0){
        
        
        bool isOverlapping = 0;
        

        if(((rectangle2TopLeftX < rectangle1BottomRightX) && (rectangle2BottomRightX > rectangle1TopLeftX)) && ((rectangle2TopLeftY < rectangle1BottomRightY )&&(rectangle2BottomRightY > rectangle1TopLeftY)) ){
            isOverlapping = 1;
        }
        else{
            return false;
        }
        if(returnOverlappingRegionUpdateBlock != 0){
            int overlapRegionTopLeftX = returnMax(rectangle2TopLeftX,rectangle1TopLeftX);
            int overlapRegionTopLeftY = returnMax(rectangle2TopLeftY,rectangle1TopLeftY);
            int overlapRegionBottomRightX = returnMin(rectangle2BottomRightX,rectangle1BottomRightX);
            int overlapRegionBottomRightY = returnMin(rectangle2BottomRightY,rectangle1BottomRightY);

            int overlapRegionWidth = overlapRegionBottomRightX-overlapRegionTopLeftX;
            int overlapRegionHeight = overlapRegionBottomRightY - overlapRegionTopLeftY;

            returnOverlappingRegionUpdateBlock->ID = overlapUpdateBlockID;
            returnOverlappingRegionUpdateBlock->width = overlapRegionWidth;
            returnOverlappingRegionUpdateBlock->height = overlapRegionHeight;
            returnOverlappingRegionUpdateBlock->Layer = overlapUpdateBlockLayer;
            returnOverlappingRegionUpdateBlock->pixelIndex = -9999; //this should be set correctly during processUpdates(), if it remains -9999 there is a problem
            returnOverlappingRegionUpdateBlock->topLeftPixelCoordinateX = overlapRegionTopLeftX;
            returnOverlappingRegionUpdateBlock->topLeftPixelCoordinateY = overlapRegionTopLeftY;
        }


        return true;

}










int binaryIntervalSearch(int* array, int sizeOfArray, int searchedElement){
    if (searchedElement < array[0] || searchedElement > array[sizeOfArray - 1]) {
        // Return -1 if the searched element is out of the bounds
        return -1;
    }

    int first = 0;
    int last = sizeOfArray - 1;

    if(searchedElement == array[last]){
        return last - 1;
    }
    while(first <= last){
        int middle = ((first +last) / 2);
        if((array[middle] <= searchedElement) && (array[middle+1] > searchedElement)){
            return middle;
        }
        else if(array[middle] < searchedElement){
            first = middle + 1;
        }
        else if (array[middle] > searchedElement){
            last = middle - 1;
        }

    }
}



    int addRedrawRegionAUX(int* renderRegions,int* numberOfRenderRegions,int p1x, int p1y, int p2x, int p2y){

        int numberOfDetectedOverlaps = 0;
            
        
        int detectedOverlapIndices[*numberOfRenderRegions];
        bool detectedSkipRegions[*numberOfRenderRegions];
        memset(detectedSkipRegions,0,sizeof(bool) * (*numberOfRenderRegions));

        while(numberOfDetectedOverlaps != 0){
            

            
            for(int i = 0; i < (*numberOfRenderRegions)*4; i = i + 4){
                if(detectedSkipRegions[i/4] == 0 && isRectangleInRectangle(p1x,p1y,p2x,p2y,renderRegions[i],renderRegions[i+1],renderRegions[i+2],renderRegions[i+3])){
                    detectedOverlapIndices[numberOfDetectedOverlaps] = i; //this is the starting index of the rectangle that is overlapped with
                    detectedSkipRegions[i/4] = 1;
                    numberOfDetectedOverlaps = numberOfDetectedOverlaps + 1;
                }
            }
            

            
            //int lowestP1x = p1x; int lowestP1y = p1y; int greatestP2x = p2x; int greatestP2y = p2y;
            for(int i =0; i < numberOfDetectedOverlaps; i++){
                if(renderRegions[detectedOverlapIndices[i]] <  p1x){
                    p1x = renderRegions[detectedOverlapIndices[i]];
                }
                if(renderRegions[detectedOverlapIndices[i] + 1]  <  p1y){
                    p1y = renderRegions[detectedOverlapIndices[i] + 1];
                }
                if(renderRegions[detectedOverlapIndices[i] + 2]  >  p2x){
                    p2x = renderRegions[detectedOverlapIndices[i] + 2];
                }
                if(renderRegions[detectedOverlapIndices[i] + 3]  >  p2y){
                    p2y = renderRegions[detectedOverlapIndices[i] + 3];
                }

                
            }
            
            
            
        }




        for(int i = numberOfDetectedOverlaps -1; i >= 0; i--){
            memmove(renderRegions + detectedOverlapIndices[i] ,renderRegions + detectedOverlapIndices[i] + 4, sizeof(int)* (( (*numberOfRenderRegions) * 4) - (detectedOverlapIndices[i] + 4)));
            *numberOfRenderRegions = *numberOfRenderRegions - 1;
        }

        renderRegions[( (*numberOfRenderRegions) *4)] = p1x; 
        renderRegions[( (*numberOfRenderRegions) *4)+1] = p1y;
        renderRegions[( (*numberOfRenderRegions) *4)+2] = p2x;
        renderRegions[( (*numberOfRenderRegions) *4)+3] = p2y;
        *numberOfRenderRegions = *numberOfRenderRegions + 1;

        return 1;

    }



struct keyBoard{
    bool isCtrlHeld(){
        return 1;
    }
    
};

struct mouseCoordinates{
    uint64_t InputSystemHeaderMarkers[3]; //this will be removed after input system gets improved
    int mousePixelCoordinateX = 0;
    int mousePixelCoordinateY = 0;
    uint64_t InputSystemFootnoteMarkers[3];

};
struct mouseButtons{
    uint64_t InputSystemHeaderMarkers[3]; //this will be removed after input system gets improved
    bool buttons[3];
    char padding[5];
    uint64_t InputSystemFootnoteMarkers[3];
};

struct mouse{
    int cursorContext = 0; //0 is defualt, 1 is click and drag element,
    int currentContextLocation[2];

    int caretActive = 0;
    int caretLocation[5]; //pane id, isrowlineregion/freeboundingboxid, element, rowlineindex within element (its local relative rowline index)

    mouseCoordinates mouseXY;
    mouseCoordinates mouseXY_Stores[4];
    updateBlock currentCursorUpdateBlock[4] = {}; //0 is mouse cursor, 1 is click and drag element, 2 is vertical pane resize, 3 is horizontal pane resize

    mouseButtons mouseKeys;
    mouseButtons mouseKeys_Stores[4];


    mouseCoordinates previousMouseXY;
    updateBlock PreviousCursorUpdateBlock = {};



    bool leftClickHeldLastFrame = 0; //when the mouseKey[0] is pressed it will set this to 1, the next frame will use this to pass an if guard, depending on ifthe mouse was released, set this variable back to 0 or keep it at 1
    //initialMouseClickXY = when mouse button is still held down and mouse moves, this tracks where the initial position of the click occured, this is needed for click
    //                      and drag multiselect
    mouseCoordinates initialMouseClickXY; 
    int initialMouseClickLocation[4];

    double previousClickTime; //if a click is registered and leftClickHeldLastFrame == 0, then the click time is recorded for the purpose of tracking if a double click occurs on the next click (after the first click was released of course)
    
    
    
    //int isMouseSelectingElement2 = 0;
    //int mouseSelectedElementLocation2[2][4];

    

   int locationDepth[2];
   int locationData[2][4];

   int isSelectingScrollbar = 0; 

    void initializeMouse(int initialMouseX, int initialMouseY){

        mouseXY.mousePixelCoordinateX = initialMouseX;
        mouseXY.mousePixelCoordinateY = initialMouseY;
        

        currentCursorUpdateBlock[0].ID = -9999;
        currentCursorUpdateBlock[0].width = 14;
        currentCursorUpdateBlock[0].height = 20;
        currentCursorUpdateBlock[0].topLeftPixelCoordinateX = mouseXY.mousePixelCoordinateX;
        currentCursorUpdateBlock[0].topLeftPixelCoordinateY = mouseXY.mousePixelCoordinateY;
        
        //currentCursorUpdateBlock[1].ID = -9998;
        currentCursorUpdateBlock[1].width = 75;
        currentCursorUpdateBlock[1].height = 40;

        currentCursorUpdateBlock[2].ID = -9998;
        currentCursorUpdateBlock[2].width = 10;
        currentCursorUpdateBlock[2].height = 20;

        currentCursorUpdateBlock[3].ID = -9997;
        currentCursorUpdateBlock[3].width = 20;
        currentCursorUpdateBlock[3].height = 10;
        


        PreviousCursorUpdateBlock.ID = -6666;

       

       

        locationData[0][0] = -1;
        locationData[0][1] = -1;
        locationData[0][2] = -1;
        locationData[0][3] = -1;

        locationData[1][0] = -1;
        locationData[1][1] = -1;
        locationData[1][2] = -1;
        locationData[1][3] = -1;

        currentContextLocation[0] = -1;
        currentContextLocation[1] = -1;

    }

    void freeMouse(){

    }
   
   
    


    //locationData[0] is the current location data, locationData[1] is the location data of the last rendererd frame
    //locationData[x][0] is the pane Index, -1 if the mouse is not in a pane
    //locationData[x][1] indicates whether the mouse is in a rowLineRegion, a scrollbar, or a freebounding box, this determines how the next two elements are interpreted
    //locationData[x][2] this is either: the index of the rowLineCluster the mouse is in, where the mouse is located in scrollbar (in bar or in scroll region), or
    //                   the free bounding box  
    //locationData[x][3] this will always indicate the index of the uiElement within the rowLineCluster  


    //flag decides which locations to compare
    //0 compares current location and current mouse selected
    //1 compares previous location and current mouse selected
    //2 compares current and previous mouse locations
    //3 compares current and previous mouse locations                             
    bool isSameLocation(int flag){
        if(flag == 2){
            printf("isSameLocation() : flag 2 \n");
            printf("bool = %d \n",(locationData[0][0] == locationData[1][0]) &&
                (locationData[0][1] == locationData[1][1]) &&
                (locationData[0][2] == locationData[1][2]) &&
                (locationData[0][3] == locationData[1][3]));

            return (locationData[0][0] == locationData[1][0]) &&
                (locationData[0][1] == locationData[1][1]) &&
                (locationData[0][2] == locationData[1][2]) &&
                (locationData[0][3] == locationData[1][3]);

        }
        

        
        

    }

    //if positionXY == 0, then isSamePosition() returns true if mouseXY position from last frame is the same as the current frame
    //if positionXY is nonzero, it is assumed to be a pointer to a int[2] array corresponding to a position to check the current frame mouse position against
    bool isSamePosition(int* positionXY = 0){
        if(positionXY == 0){
            return (mouseXY.mousePixelCoordinateX == previousMouseXY.mousePixelCoordinateX) && (mouseXY.mousePixelCoordinateY == previousMouseXY.mousePixelCoordinateY);
        }

    }


    void setMouseCursorContext(int flag, int clickAndDragElementID = 0){
        cursorContext = flag;
        if(flag == 0){ //default cursor
            
        }
        if(flag == 1){ //click and drag element
            currentCursorUpdateBlock[1].ID = clickAndDragElementID;
        }
        if(flag == 2){ //click and drag multiselect

        }
    }

    void render(){

    }

     

    
    



};



//each text (letter) box is part of a "TEXT BOX"                //each button,icon,background color(excluding panes),etc... is represented as a "UI ELEMENT"
//each "TEXT BOX" is part of a "ROW LINE ALGINMENT"             //each "UI ELEMENT" is part of a "ROW LINE ALIGNMENT"
//each "ROW LINE ALIGNMENT is part of a "UI ELEMENTROW"         //each "ROW LINE ALIGNMENT is part of a UI ELEMENTROW"
//each "UI ELEMENTROW" is part of a panel                       //each "UI ELEMENTROW" is part of a panel

//There are only two exceptions to the above

#define TEXTBOX_BUFFER_SIZE 20
#define ROWLINEALIGNMENT_BUFFER_SIZE 10
class textBox{
    public:
    int RowLineAlignmentPixelCoordinateX;
    int RowLineAlignmentPixelCoordinateY;
    //this is reset to 0 by the ui object, 1 indicates a general refresh and the ui object must discern what needs done
    //                                     2 indicates only appends have been made
    //                                     3 indicates only truncations have been made
    //                                     4 indicates only 1 insertText has been made (this makes use of the lastValidRefreshIndex variable)
    //                                     5 indicates only 1 removeText has been made (this makes use of the lastValidRefreshIndex variable)

    //                                     in any other scenario needsRefresh should be set to 1, and the ui object will discern needed updates
    //                                     solely using the lastFrameTextBuffer* data and comparing it with the textBuffer* data
    int NeedsRefresh = 0; 
    int LastValidRefreshIndex = 0;
    
    int PreviousFrameRowLineAlignmentPixelCoordinateX;
    int PreviousFrameRowLineAlignmentPixelCoordinateY;
    int PreviousFrameTextBufferIndex = 0;
    char* PreviousFrameTextBuffer;
    int PreviousFrameWidth;
    int PreviousFrameHeight;


    int TextBufferSize = 0;
    bool Selected = 0;
    int TextBufferIndex = 0;
    char* TextBuffer;
    int highlighted = 0; //1 is used for mouse hover, 2 is used for when selected

    int* GlyphPixelCoordinatesX;

    int FontSize;

    int Layer;
    //width is determined by subtracting rightmost text pixel by leftmost text pixel
    //height is determined by subtracting the ROW LINE ALIGNMENT y position by the top
    int Width = 0;
    int Height = 0;

    void horizontalShift(int shiftAmount){
        RowLineAlignmentPixelCoordinateX = RowLineAlignmentPixelCoordinateX + shiftAmount;
        for(int i = 0; i < TextBufferIndex; i++){
            GlyphPixelCoordinatesX[i] = GlyphPixelCoordinatesX[i] + shiftAmount;
        }
    }


    void appendText(char* inputArray, int sizeOfInputArray){
       if(NeedsRefresh == 0){
            NeedsRefresh = 1;
        }
       if(TextBufferIndex == LastValidRefreshIndex){
            LastValidRefreshIndex = TextBufferIndex;
        }   

        for(int i = 0; i < sizeOfInputArray; i++){
            TextBuffer[TextBufferIndex] = inputArray[i];
            //Width = Width + getGlyphWidth()
            //if(getGlyphHeight() > Height){Height = getGlyphHeight()}
            TextBufferIndex = TextBufferIndex + 1;
        }

    }

//te


    void truncateText(int numberOfLettersToTruncate){
        
        if(NeedsRefresh == 0){
            NeedsRefresh = 1;
        }
        if(TextBufferIndex == LastValidRefreshIndex){
            LastValidRefreshIndex = TextBufferIndex - numberOfLettersToTruncate;
        }   
        for(int i = 0; i < numberOfLettersToTruncate; i++){
            //Width = Width - getGlyphWidth(TextBuffer[textBufferIndex])
            //if(getGlyphHeight() == Height){recalculateHeight()}
            TextBufferIndex = TextBufferIndex - 1;
        }
    }

    void insertText(char* inputArray, int sizeOfInputArray,int insertionIndex){
        /*
        for(int i = 0; i < sizeOfInputArray; i++){
            TextBuffer[insertionIndex] = inputArray[i];
            TextBufferIndex = TextBufferIndex + 1;
        }
        */
        
    }

    void removeText(){}

    void moveTextBox(int offsetX, int offsetY){
        RowLineAlignmentPixelCoordinateX = RowLineAlignmentPixelCoordinateX + offsetX;
        RowLineAlignmentPixelCoordinateY = RowLineAlignmentPixelCoordinateY + offsetY;
    }

    textBox(){
        RowLineAlignmentPixelCoordinateX = -9999;
        RowLineAlignmentPixelCoordinateY = -9999;
        FontSize = 0;

    }

    int intitializeTextBox(int textBufferSize,int rowLineAlignmentPixelCoordinateX,int rowLineAlignmentPixelCoordinateY,int fontSize,int layer,char* initialText=0 ,int initialTextSize=0,int textBoxMaxWidth = -1){
        //printf("\t \t \t intitializeTextBox() : \n");
        TextBufferSize = textBufferSize;
        TextBufferIndex = 0;
        RowLineAlignmentPixelCoordinateX = rowLineAlignmentPixelCoordinateX;
        RowLineAlignmentPixelCoordinateY = rowLineAlignmentPixelCoordinateY;
        FontSize = fontSize;
        Layer = layer;
        Width = 0;

        if(textBufferSize > 0){
            TextBuffer = (char*)malloc(textBufferSize);
            GlyphPixelCoordinatesX = (int*)malloc(textBufferSize);

            PreviousFrameTextBuffer = (char*)malloc(textBufferSize); 
            

            
            if((initialText != 0) && (initialTextSize != 0)){
                int i = 0;
                for(i; i < initialTextSize && i < textBufferSize; i++){
                    TextBuffer[i] = initialText[i];
                    GlyphPixelCoordinatesX[i] =  RowLineAlignmentPixelCoordinateX + Width;
                    //printf("\t \t \t intitializeTextBox() : for{} : i = %d : TextBuffer[i] = %d , GlyphPixelCoordinatesX[i] = %d, TextBufferIndex = %d \n",i,TextBuffer[i],GlyphPixelCoordinatesX[i],TextBufferIndex);
                    Width = Width + getGlyphWidth(initialText[i]);
                    if(textBoxMaxWidth > 0 && Width > textBoxMaxWidth){
                        Width = Width - getGlyphWidth(initialText[i]);
                        //i = i - 1;
                        //printf("\t \t \t intitializeTextBox() : return path 1 \n");
                        return initialTextSize - i;


                    }
                    if(getGlyphHeight(initialText[i]) > Height){
                        Height = getGlyphHeight(initialText[i]);
                    }
                    TextBufferIndex = TextBufferIndex + 1;

                }

                


                if((i == textBufferSize && i < initialTextSize) ){
                    //printf("\t \t \t intitializeTextBox() : return path 2 : i = %d , textBufferSize = %d , initialTextSize = %d  \n",i,textBufferSize,initialTextSize);
                    return initialTextSize - textBufferSize;
                }
            }
        }

        else{
            TextBuffer = 0;
            GlyphPixelCoordinatesX = 0;
            PreviousFrameTextBuffer = 0;
            Height = FontSize;

        }

        //printf("\t \t \t intitializeTextBox() : return path 0 \n");
        return 0;
    }


    bool isUpdateBlockInTextBox(updateBlock inputUpdateBlock,bool* returnTextIndices){
        //printf("DEBUGGING isUpdateBlockInTextBox \n");
        //printf("DEBUGGING isUpdateBlockInTextBox : inputUpdateBlock.topLeftPixelCoordinateX = %d \n",inputUpdateBlock.topLeftPixelCoordinateX);
        //printf("DEBUGGING isUpdateBlockInTextBox : inputUpdateBlock.topLeftPixelCoordinateY = %d \n",inputUpdateBlock.topLeftPixelCoordinateY);
        int inputUpdateBlockBottomRightPixelX = inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width;
        int inputUpdateBlockBottomRightPixelY = inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height;
        //printf("DEBUGGING isUpdateBlockInTextBox : inputUpdateBlockBottomRightPixelX : %d \n",inputUpdateBlockBottomRightPixelX);
        //printf("DEBUGGING isUpdateBlockInTextBox : inputUpdateBlockBottomRightPixelY : %d \n",inputUpdateBlockBottomRightPixelY);
        int textBoxUpperLeftX = RowLineAlignmentPixelCoordinateX ;
        int textBoxUpperLeftY= RowLineAlignmentPixelCoordinateY - Height;
        int textBoxBottomRightX = RowLineAlignmentPixelCoordinateX + Width;
        int textBoxBottomRightY = RowLineAlignmentPixelCoordinateY;
        //printf("DEBUGGING isUpdateBlockInTextBox : textBoxUpperLeftX = %d \n",textBoxUpperLeftX);
        //printf("DEBUGGING isUpdateBlockInTextBox : textBoxUpperLeftY = %d \n",textBoxUpperLeftY);
        //printf("DEBUGGING isUpdateBlockInTextBox : textBoxBottomRightX = %d \n",textBoxBottomRightX);
        //printf("DEBUGGING isUpdateBlockInTextBox : textBoxBottomRightY = %d \n",textBoxBottomRightY);


      
        bool returnValue;
        if(((textBoxUpperLeftX <= inputUpdateBlockBottomRightPixelX) && (textBoxBottomRightX >= inputUpdateBlock.topLeftPixelCoordinateX)) && ((textBoxUpperLeftY <= inputUpdateBlockBottomRightPixelY )&&(textBoxBottomRightY >= inputUpdateBlock.topLeftPixelCoordinateY)) ){
            returnValue = true;
        }
        else{
            returnValue = false;
            return returnValue;
        }

        for(int i = 0; i < TextBufferIndex; i++){
            //characterUpdateBlock.width = getGlyphWidth(character);
            //characterUpdateBlock.height = getGlyphHeight(character);
            //characterUpdateBlock.topLeftPixelCoordinateX = UiPanes[i].RowLineBuffer[j].TextBoxBuffer[k].GlyphPixelCoordinatesX[l];
            //characterUpdateBlock.topLeftPixelCoordinateY = UiPanes[i].RowLineBuffer[j].TextBoxBuffer[k].RowLineAlignmentPixelCoordinateY + characterUpdateBlock.height;
            
            char textGlyph = TextBuffer[i];
            int textGlyphUpperLeftX = GlyphPixelCoordinatesX[i];
            int textGlyphUpperLeftY = RowLineAlignmentPixelCoordinateY - getGlyphHeight(textGlyph);
            int textGlyphBottomRightX = GlyphPixelCoordinatesX[i] + getGlyphWidth(textGlyph);
            int textGlyphBottomRightY = RowLineAlignmentPixelCoordinateY;
 
        
        
            if(((textGlyphUpperLeftX <= inputUpdateBlockBottomRightPixelX) && (textGlyphBottomRightX >= inputUpdateBlock.topLeftPixelCoordinateX)) && ((textGlyphUpperLeftY <= inputUpdateBlockBottomRightPixelY )&&(textGlyphBottomRightY >= inputUpdateBlock.topLeftPixelCoordinateY)) ){
                returnTextIndices[i] = 1;
                //printf("returnTextIndices[%d] = 1 \n", i);
            }
            else{
                returnTextIndices[i] = 0;
                //printf("returnTextIndices[%d] = 0 \n", i);

            }


            
        }

        return returnValue;


    }


    int AddRedrawRegion(bool* textUpdateIndices){
        
    }
    

    void render(uiData* ui,rowLineRegion* parentRowLineRegion);

    void deleteTextBox(){
        free(TextBuffer);
        free(GlyphPixelCoordinatesX);
        free(PreviousFrameTextBuffer);
    }

   

};


class VisualElement{
    public:
    int RowLineAlignmentPixelCoordinateX;
    int RowLineAlignmentPixelCoordinateY;
    int NeedsRefresh = 0;
    int highlighted = 0; //1 means hovered, 2 means selected

    int PreviousFrameRowLineAlignmentPixelCoordinateX;
    int PreviousFrameRowLineAlignmentPixelCoordinateY;
    int PreviousFrameWidth;
    int PreviousFrameHeight;

    
    int ElementID;
    int Layer;
    int Width;
    int Height;

    void horizontalShift(int shiftAmount){
        RowLineAlignmentPixelCoordinateX = RowLineAlignmentPixelCoordinateX + shiftAmount;

    }

    VisualElement(){
        RowLineAlignmentPixelCoordinateX = -9999;
        RowLineAlignmentPixelCoordinateY = -9999;
        ElementID = -9999;
        Layer = -9999;
        Width = 0;
        Height = 0;
        
    }

    VisualElement(int elementID,int rowLineAlignmentPixelCoordinateX,int rowLineAlignmentPixelCoordinateY,int layer, int width, int height){
        ElementID = elementID;
        RowLineAlignmentPixelCoordinateX = rowLineAlignmentPixelCoordinateX;
        RowLineAlignmentPixelCoordinateY = rowLineAlignmentPixelCoordinateY;
        //printf("DEBUGGING uiELEMENT() : elementID = %d : rowLineAlignmentPixelCoordinateX = %d : rowLineAlignmentPixelCoordinateY = %d \n",elementID,rowLineAlignmentPixelCoordinateX,rowLineAlignmentPixelCoordinateY);
        Layer = layer;
        Width = width;
        Height = height;
    }
    void moveVisualElement(int offsetX, int offsetY){
        NeedsRefresh = 1;
        RowLineAlignmentPixelCoordinateX = RowLineAlignmentPixelCoordinateX + offsetX;
        RowLineAlignmentPixelCoordinateY = RowLineAlignmentPixelCoordinateY + offsetY;
    }


    bool isUpdateBlockInUiElement(updateBlock inputUpdateBlock, updateBlock* returnOverlapRegion){
        int inputUpdateBlockBottomRightPixelX = inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width;
        int inputUpdateBlockBottomRightPixelY = inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height;

        int uiElementUpperLeftX = RowLineAlignmentPixelCoordinateX;
        int uiElementUpperLeftY = RowLineAlignmentPixelCoordinateY - Height;
        int uiElementBottomRightX = RowLineAlignmentPixelCoordinateX + Width;
        int uiElementBottomRightY = RowLineAlignmentPixelCoordinateY;

      return isRectangleInRectangle(inputUpdateBlock.topLeftPixelCoordinateX, inputUpdateBlock.topLeftPixelCoordinateY, inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width, inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height,
                             uiElementUpperLeftX,uiElementUpperLeftY,uiElementBottomRightX,uiElementBottomRightY,returnOverlapRegion,0,0);
        
        /*
        if(((uiElementUpperLeftX <= inputUpdateBlockBottomRightPixelX) && (uiElementBottomRightX >= inputUpdateBlock.topLeftPixelCoordinateX)) && ((uiElementUpperLeftY <= inputUpdateBlockBottomRightPixelY )&&(uiElementBottomRightY >= inputUpdateBlock.topLeftPixelCoordinateY)) ){
            return true;
        }
        else{
            return false;
        }
        */

    }

    int AddRedrawRegion(){

    }

    void render(uiData* ui, rowLineRegion* parentRowLineRegion = 0);

};



class rowLineAlignment{
    public:
    int Layer;
    int needsRefresh; //ONLY used for FREE rowlines, not rowlines belonging to a rowline region
    int PixelCoordinateY;
    int TextBoxBufferIndex = 0;
    textBox* TextBoxBuffer;

    int VisualElementBufferIndex = 0;
    VisualElement* VisualElementBuffer;
    
    int RowLineBoundingHeight = -9999; //max height in the row line

    //textBoxOrUiElement == 0 MEANS CREATE uiElementBuffer, textBoxOrUiElement == 1 MEANS CREATE textBoxBuffer
    rowLineAlignment(bool textBoxOrVisualElement,int bufferSize,int layer, int pixelCoordinateY){
        if(textBoxOrVisualElement){
            VisualElementBuffer = (VisualElement*)malloc(sizeof(VisualElement) * bufferSize);
            TextBoxBuffer = 0;
        }
        else{
            TextBoxBuffer = (textBox*)malloc(sizeof(textBox) * bufferSize);
            VisualElementBuffer = 0;

        }
        Layer = layer;
        PixelCoordinateY = pixelCoordinateY;
    }

    int createTextBox(int textBoxBufferSize,int rowLineXOffset,int fontSize,char* initialText=0 ,int initialTextSize=0,int textBoxMaxWidth=-1){
        printf("\t \t createTextBox() : \n ");
        printf("\t \t createTextBox() : initialTextSize = %d \n ", initialTextSize);
        int returnVal = TextBoxBuffer[TextBoxBufferIndex].intitializeTextBox(textBoxBufferSize,rowLineXOffset,PixelCoordinateY,fontSize,Layer,initialText,initialTextSize,textBoxMaxWidth);
        TextBoxBufferIndex = TextBoxBufferIndex + 1;
        printf("\t \t createTextBox() : returnVal = %d, TextBoxBufferIndex \n ",returnVal,TextBoxBufferIndex);
        return returnVal;
    }

    void createVisualElement(int rowLineXOffset,int elementID,int width, int height){
        VisualElementBuffer[VisualElementBufferIndex] = VisualElement(elementID,rowLineXOffset,PixelCoordinateY,Layer,width,height);
        VisualElementBufferIndex = VisualElementBufferIndex + 1;
    }

    void moveRowLineAlignment(int offsetX, int offsetY){
        
        PixelCoordinateY = PixelCoordinateY + offsetY;
        for(int i = 0; i < TextBoxBufferIndex; i++){
            VisualElementBuffer[i].moveVisualElement(offsetX,offsetY);
            TextBoxBuffer[i].moveTextBox(offsetX,offsetY);
        }

    }

    void updateRowLineBoundingHeight(){
        //printf("DEBUGGING updateRowLineBoundingHeight() : \n");
        //printf("DEBUGGING updateRowLineBoundingHeight() : RowLineBoundingHeight : %d \n", RowLineBoundingHeight);
        //printf("DEBUGGING updateRowLineBoundingHeight() : TextBoxBuffer != 0 : %d\n", (TextBoxBuffer != 0));
        //printf("DEBUGGING updateRowLineBoundingHeight() : TextBoxBufferIndex = %d \n", TextBoxBufferIndex);
        //printf("DEBUGGING updateRowLineBoundingHeight() : TextBoxBuffer[0] = %d \n", TextBoxBuffer[0].Height);

        if(TextBoxBuffer != 0){
            for(int i = 0; i < TextBoxBufferIndex; i++){
                if(TextBoxBuffer[i].Height > RowLineBoundingHeight){
                    RowLineBoundingHeight = TextBoxBuffer[i].Height;
                    //printf("DEBUGGING updateRowLineBoundingHeight() : RowLineBoundingHeight : %d \n", RowLineBoundingHeight);
                }
            }
        }
        else if(VisualElementBuffer != 0){
            for(int i = 0; i < VisualElementBufferIndex; i++){
                if(VisualElementBuffer[i].Height > RowLineBoundingHeight){
                    RowLineBoundingHeight = VisualElementBuffer[i].Height;
                }
            }
        }
    }


    bool isUpdateBlockInRowLine(updateBlock inputUpdateBlock){
        //printf("DEBUGGING isUpdateBlockInRowLine \n");
        //printf("DEBUGGING isUpdateBlockInRowLine : UiPanes[paneIndex].RowLineBuffer[rowLineIndex].TextBoxBuffer[0].Height = %d \n", UiPanes[paneIndex].RowLineBuffer[rowLineIndex].TextBoxBuffer[0].Height);
    
        //UiPanes[paneIndex].RowLineBuffer[rowLineIndex].updateRowLineBoundingHeight();
        updateRowLineBoundingHeight();

        int inputUpdateBlockBottomRightPixelX = inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width;
        int inputUpdateBlockBottomRightPixelY = inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height;
        //printf("DEBUGGING isUpdateBlockInRowLine : inputUpdateBlockBottomRightPixelX : %d \n",inputUpdateBlockBottomRightPixelX);
        //printf("DEBUGGING isUpdateBlockInRowLine : inputUpdateBlockBottomRightPixelY : %d \n",inputUpdateBlockBottomRightPixelY);


        int rowLineUpperLeftX = 0;
        int rowLineUpperLeftY = PixelCoordinateY - RowLineBoundingHeight;
        

        int rowLineBottomRightX = 4096; //i just use a large number here it doesnt really matter, the row line area can be imagined to extend horizontally forever
        int rowLineBottomRightY = PixelCoordinateY;
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineUpperLeftX : %d \n",rowLineUpperLeftX);
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineUpperLeftY : %d \n",rowLineUpperLeftY);
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineBottomRightX : %d \n",rowLineBottomRightX);
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineBottomRightY : %d \n",rowLineBottomRightY);
        
        if(((rowLineUpperLeftX <= inputUpdateBlockBottomRightPixelX) && (rowLineBottomRightX >= inputUpdateBlock.topLeftPixelCoordinateX)) && ((rowLineUpperLeftY <= inputUpdateBlockBottomRightPixelY )&&(rowLineBottomRightY >= inputUpdateBlock.topLeftPixelCoordinateY)) ){
            return true;
        }
        else{
            return false;
        }
        



    }

    void render(uiData* ui, rowLineRegion* parentRowLineRegion);

    void deleteRowLineAlignment(){
        if(TextBoxBuffer){
            for(int i = 0; i < TextBoxBufferIndex; i++){
                TextBoxBuffer[i].deleteTextBox();
            }
            free(TextBoxBuffer);
        }
        else{
            free(VisualElementBuffer);

        }

    }

};



class scrollBar{
    public:
    int Type = -9999; //this is set to -9999 until initializeScrollbar(), this allows us to use Type to see if a scrollbar is uninitialized
    int ID;
    int Layer;
    bool HorizontalOrVertical;
    bool needsRefresh;
    int highLighted = 0; //1 means hovering, 2 means selected
    //Its important to note that the scrollableRegion variables are used directly for rendering when submitting updateBlocks*, As Well As signifying the
    //Start and End positions of the Slider. A Slider is expected to Start at the TopLeftPixel and End at the BottomRightPixel.

    //*scrollBarSliderSize and scrollBarEndOfSliderPosition are Also used directly for rendering when submitting updateBlocks.*
    int ScrollableRegionTopLeftPixelX;
    int ScrollableRegionTopLeftPixelY;
    int ScrollableRegionBottomRightPixelX;
    int ScrollableRegionBottomRightPixelY;

    int ScrollableRegionWidth;
    int ScrollableRegionHeight;

    int ScrollBarSliderSize; //sliderEndPosition - sliderStartPosition + 1

    // scrollBarSize = sliderEnd - sliderStart + 1
    // sliderStart = sliderEnd - scrollBarSize + 1
    // sliderEnd = scrollBarSize + sliderStart - 1

   
    


    int ScrollBarStartOfSliderPosition;
    int ScrollBarEndOfSliderPosition; //this is treated as a X Pixel Coordinate in a horizontal scrollbar, and a Y Pixel Coordinate in a vertical scrollbar
    float ScrollPercent = 0.0;        //This percentage represents how far the EndOfSliderPosition is from the end of the scrollable region

    int NumberOfRenderRegions[2];
    int* RenderRegion[2];

    

    void initializeScrollbar(bool horizontalOrVertical, int topLeftPixelX, int topLeftPixelY, int bottomRightPixelX, int bottomRightPixelY,int scrollBarSliderSize,int layer){
        ID = 200'001;
        Type = horizontalOrVertical;

        ScrollableRegionTopLeftPixelX = topLeftPixelX;
        ScrollableRegionTopLeftPixelY = topLeftPixelY;
        ScrollableRegionBottomRightPixelX = bottomRightPixelX;
        ScrollableRegionBottomRightPixelY = bottomRightPixelY;

        ScrollBarSliderSize = scrollBarSliderSize;

        ScrollableRegionWidth = ScrollableRegionBottomRightPixelX - ScrollableRegionTopLeftPixelX;
        ScrollableRegionHeight = ScrollableRegionBottomRightPixelY - ScrollableRegionTopLeftPixelY;

        if(HorizontalOrVertical == 0){
            ScrollBarStartOfSliderPosition = ScrollableRegionTopLeftPixelY;
            //ScrollBarEndOfSliderPosition = ScrollableRegionBottomRightPixelY;
            ScrollBarEndOfSliderPosition = ScrollBarStartOfSliderPosition + ScrollBarSliderSize;
        }

        highLighted = 0;
        ScrollPercent = 0.0;

        needsRefresh = 1;
        Layer = layer;
    }


/*
    //0 means no hovering over scrollbar region at all,
    //1 means hovering over scrollbar region, 2 means hovering over slider
    int isMouseHovering(int mousePixelCoordinateX, int mousePixelCoordinateY){
        return (mousePixelCoordinateX >= ScrollableRegionTopLeftPixelX) && (mousePixelCoordinateX <= ScrollableRegionBottomRightPixelX) && (mousePixelCoordinateY >= ScrollableRegionTopLeftPixelY) && (mousePixelCoordinateY <= ScrollableRegionBottomRightPixelY);

    }
*/

    int findMouseLocation(mouse* uiMouse){
        uiMouse->locationData[0][1] = -1;
        uiMouse->locationData[0][2] = -1;
        
        //check if in scrollbar region
        if(isPointInRectangle(uiMouse->mouseXY.mousePixelCoordinateX,uiMouse->mouseXY.mousePixelCoordinateY,ScrollableRegionTopLeftPixelX,ScrollableRegionTopLeftPixelY,ScrollableRegionBottomRightPixelX,ScrollableRegionBottomRightPixelY)){
            uiMouse->locationData[0][1] = 1+Type;
            uiMouse->locationDepth[0] = 1;

            //check to see if in scrollbar slider
            if(Type == 0){
                
                if(isPointInRectangle(uiMouse->mouseXY.mousePixelCoordinateX,uiMouse->mouseXY.mousePixelCoordinateY,ScrollableRegionTopLeftPixelX,ScrollBarStartOfSliderPosition,ScrollableRegionBottomRightPixelX,ScrollBarEndOfSliderPosition)){
                    //printf("\n\n\n\n FOUND IT FOUND THE PROBLEM \n\n\n\n");
                    uiMouse->locationData[0][2] = 1;
                    uiMouse->locationDepth[0] = 2;
                    return 2;
                }

            }
            if(Type == 1){
                if(isPointInRectangle(uiMouse->mouseXY.mousePixelCoordinateX,uiMouse->mouseXY.mousePixelCoordinateY,ScrollableRegionTopLeftPixelX,ScrollableRegionTopLeftPixelY,ScrollableRegionBottomRightPixelX,ScrollableRegionBottomRightPixelY)){
                    //printf("\n\n\n\n FOUND IT FOUND THE PROBLEM 2 \n\n\n\n");
                    uiMouse->locationData[0][2] = 1;
                    uiMouse->locationDepth[0] = 2;
                    return 2;
                }
            }

            return 1;

            


            

        }

        return 0;

    }

    bool isMouseSelectingSlider(){

    }

    


    //to scroll i would need to find out what offset would be needed to get the bottom rowLine onto the minimum/bottom of the scroll region
    //this would require finding the 

    //this would require sorting of the rowLineBuffer with respect to positioning

    //offset being positive moves the slider from start to end, offet being negative moves slider from end to start (backwards)
    void moveSlider(int offset){

        if(HorizontalOrVertical == 0){


        }

        else{

            if(offset>0){
                if(ScrollPercent == 1.0){
                    return;
                }
                ScrollBarStartOfSliderPosition = ScrollBarStartOfSliderPosition + offset;
                ScrollBarEndOfSliderPosition = ScrollBarEndOfSliderPosition + offset;
                if(ScrollBarEndOfSliderPosition > ScrollableRegionBottomRightPixelY){
                    ScrollBarEndOfSliderPosition = ScrollableRegionBottomRightPixelY;
                    ScrollBarStartOfSliderPosition = ScrollBarEndOfSliderPosition - ScrollBarSliderSize + 1;

                }
            }

            else{
                if(ScrollPercent == 0.0){
                    return;
                }
                ScrollBarStartOfSliderPosition = ScrollBarStartOfSliderPosition - offset;
                ScrollBarEndOfSliderPosition = ScrollBarEndOfSliderPosition - offset;
                if(ScrollBarStartOfSliderPosition < ScrollableRegionTopLeftPixelY){
                    ScrollBarStartOfSliderPosition = ScrollableRegionTopLeftPixelY;
                    ScrollBarEndOfSliderPosition = ScrollBarSliderSize + ScrollBarStartOfSliderPosition - 1;


                }

            }

        }

        //                                                ||          initial endOfSliderPosition            ||                
        ScrollPercent = (ScrollBarEndOfSliderPosition -   (ScrollableRegionTopLeftPixelY + ScrollBarSliderSize) ) 
                        /
                        (ScrollableRegionBottomRightPixelY - (ScrollableRegionTopLeftPixelY + ScrollBarSliderSize));


    }

    int addRedrawRegion(int p1x, int p1y, int p2x, int p2y,bool scrollBarRegionOrScrollBarSlider = 0){

        addRedrawRegionAUX(RenderRegion[scrollBarRegionOrScrollBarSlider],NumberOfRenderRegions + scrollBarRegionOrScrollBarSlider,
                           p1x,p1y,p2x,p2y);


    }


    



};




//This struct configures what a rowLineRegion should interpret an "element" to be/consist of
struct rowLineRegionElementInfo{
    int numberOfRowLinesPerElement;
    int numberOfTextBoxesInElement;
    int numberOfVisualElementsInElement;
    VisualElement* VisualElements;
    textBox* textBoxes;
    bool* ordering; //an array that specifies the order in which to read from textboxes or visualElements, when adding an element. A value of 0 siginifies a textbox, whereas 1 signifies a visualElement. This array Should be size of numberOfRowLinesPerElement
    bool* textBoxWrapAround; // If ordering[i] == 0, Then textBoxWrapAround[i] signifies whether or not this textbox requires a wrap around whenever it exceeds either its textBoxMaxWidth[i] or textBoxMaxLength[i]. This array Should be size of numberOfRowLinesPerElement
    int* textBoxWrapAroundSpacing; //If ordering[0] == 0 AND textBoxWrapAround[i] == 1, Then this signifies the vertical spacing to be applied to the textbox needing wraparound and the new textbox continuation beneath
    int* textBoxMaxWidth; // If ordering[i] == 0, Then textBoxMaxWidth[i] signifies the max width this textbox can be (measured by summing each glyphs width + spacings) before requiring a cutoff or a textBoxWrapAround. This array Should be size of numberOfRowLinesPerElement
    int* textBoxMaxStringLength; //If ordering[i] == 0, Then textBoxMaxStringLength[i] signifies the max string length a textbox can contain before requiring cutoff or textbox wrap around

    // an array that specifies how a visualElement/textBox should be centered in a uiElement, if this value is 9999 then it signifies to center about the
    // rowlineCluster's elementWidth, if the values is -1 then no centering is applied, if the value is >= 0 then this signifies the inde of the textbox/uiElement
    // that this textbox/uiElement is to be centered on
    int* centerOn_index; 



    int firstElementPixelCoordinateX;
    int firstElementPixelCoordinateY;

    int horizontalSpacing;
    int verticalSpacing;

    void deleteRowLineRegionElementInfo(){
        if(VisualElements){
            free(VisualElements);
        }
        for(int i = 0; i < numberOfTextBoxesInElement; i++){
            textBoxes[i].deleteTextBox();
        }
        free(textBoxes);
       
    }

};





class rowLineCluster{
    public:
    rowLineRegion* ParentRowLineRegion;
    rowLineAlignment* RowLineBuffer;
    bool* Ordering;
    int StartingRowLineIndex;//raw
    int ClusterIndex; //raw

    //int VirtualStartingRowLineIndex;
    int VirtualClusterIndex;



    int NumberOfRowLinesInCluster = 0;
    

    int NumberOfElementsInCluster = 0;
    int* NumberOfRowLinesPerElement;
    

    rowLineRegionElementInfo* ElementPrototype;
    int ClusterVerticalOffset = -9999;

    int ElementWidth = 0;
    int ElementBottomSideYValue = 0;
    int ElementTopSideYValue = 0;
    int ElementHeight = 0;


    int HorizontalSpacing;
    int StartingElementPositionX;

    int* ElementStartingX;



    //Remember that highlightElement() pertains to mouse hover, mouse select, and even system highlights/selects. It pertains to all general instances of the element being statefully and visually affected by interaction
    int highlightElement(int elementIndex, int highlightVal){
        //StartingRowLineIndex
        //RowLineBuffer
        printf("void highlightElement() : elementIndex = %d, highlightVal = %d \n",elementIndex, highlightVal);
        
        printf("void highlightElement() : (RowLineBuffer[StartingRowLineIndex].uiElementBuffer[elementIndex].highlighted > 1) = %d \n",(RowLineBuffer[StartingRowLineIndex].VisualElementBuffer[elementIndex].highlighted > 1));
        if(highlightVal < 2 && ((RowLineBuffer[StartingRowLineIndex].VisualElementBuffer && (RowLineBuffer[StartingRowLineIndex].VisualElementBuffer[elementIndex].highlighted > 1)) || (RowLineBuffer[StartingRowLineIndex].TextBoxBuffer  && (RowLineBuffer[StartingRowLineIndex].TextBoxBuffer[elementIndex].highlighted > 1)))){
            printf("if(highlightVal < 2 && ((RowLineBuffer[StartingRowLineIndex].uiElementBuffer && (RowLineBuffer[StartingRowLineIndex].uiElementBuffer[elementIndex].highlighted > 1)) || (RowLineBuffer[StartingRowLineIndex].TextBoxBuffer  && (RowLineBuffer[StartingRowLineIndex].TextBoxBuffer[elementIndex].highlighted > 1)))){} \n");
            return -1;
        }
        if(highlightVal == 2 && ((RowLineBuffer[StartingRowLineIndex].VisualElementBuffer && (RowLineBuffer[StartingRowLineIndex].VisualElementBuffer[elementIndex].highlighted > 1)) || (RowLineBuffer[StartingRowLineIndex].TextBoxBuffer  && (RowLineBuffer[StartingRowLineIndex].TextBoxBuffer[elementIndex].highlighted > 1)))){
            return -2;
        }
        if(highlightVal == 9999){ //force deselect (needed to signal highlightVal = 0 to a selected element)
            printf("void highlightElement() : if(highlightVal == 9999){}  \n");
            highlightVal = 0;

        }
        if(highlightVal == 9998){
            printf("void highlightElement() : if(highlightVal == 9999){}  \n");
            highlightVal = 2;
            
        }
        

        printf("void highlightElement() : NumberOfRowLinesPerElement[elementIndex] = %d  \n",NumberOfRowLinesPerElement[elementIndex]);
        for (int i = 0; i < NumberOfRowLinesPerElement[elementIndex]; i++) {
            printf("void highlightElement() : for {} : i = %d  \n",i);
            printf("void highlightElement() : for {} : ElementPrototype->numberOfRowLinesPerElement = %d  \n",ElementPrototype->numberOfRowLinesPerElement);
                if(i >= ElementPrototype->numberOfRowLinesPerElement){
                    printf("void highlightElement() : for {} : if(i >= ElementPrototype->numberOfRowLinesPerElement){} \n");
                    int rowLineBufferIndexStart = StartingRowLineIndex;
                    printf("void highlightElement() : rowLineBufferIndexStart = %d \n",rowLineBufferIndexStart);
                    printf("void highlightElement() : RowLineBuffer[rowLineBufferIndexStart+i].TextBoxBufferIndex = %d \n",RowLineBuffer[rowLineBufferIndexStart+i].TextBoxBufferIndex);
                    for(int j = 0; j < RowLineBuffer[rowLineBufferIndexStart+i].TextBoxBufferIndex; j++){
                        printf("void highlightElement() : for int j = %d \n",j);
                        if((RowLineBuffer[rowLineBufferIndexStart+i].TextBoxBuffer[j].GlyphPixelCoordinatesX[0] >= ElementWidth * j) && (RowLineBuffer[rowLineBufferIndexStart+i].TextBoxBuffer[j].GlyphPixelCoordinatesX[0] < ElementWidth * (j+1))){
                            printf("void highlightElement() : for int j = %d : if{} REACHED \n",j);
                            elementIndex = j;
                        }
                    }
                    elementIndex = 0;
                }
                
                printf("void highlightElement() : for {} : i = %d : StartingRowLineIndex = %d \n", i, StartingRowLineIndex);
                if (RowLineBuffer[StartingRowLineIndex+i].VisualElementBuffer) {
                    printf("void highlightElement() : for {} : i = %d : if (RowLineBuffer[StartingRowLineIndex+i].uiElementBuffer) {} \n",i);
                    RowLineBuffer[StartingRowLineIndex+i].VisualElementBuffer[elementIndex].NeedsRefresh = 1;
                    RowLineBuffer[StartingRowLineIndex+i].VisualElementBuffer[elementIndex].highlighted = highlightVal;
                    printf("void highlightElement() : for {} : i = %d : if ... : RowLineBuffer[StartingRowLineIndex+i].uiElementBuffer[elementIndex].NeedsRefresh = %d \n",i,RowLineBuffer[StartingRowLineIndex+i].VisualElementBuffer[elementIndex].NeedsRefresh);
                    printf("void highlightElement() : for {} : i = %d : if... : RowLineBuffer[StartingRowLineIndex+i].uiElementBuffer[elementIndex].highlighted= %d  \n",i,RowLineBuffer[StartingRowLineIndex+i].VisualElementBuffer[elementIndex].highlighted);
                    
                }
                else if (RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer) {
                    printf("void highlightElement() : for {} : i = %d : if (RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer) {} \n",i);
                    RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer[elementIndex].NeedsRefresh = 1;
                    RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer[elementIndex].highlighted = highlightVal;
                    printf("void highlightElement() : for {} : i = %d : if ... : RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer[elementIndex].NeedsRefresh = %d \n",i,RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer[elementIndex].NeedsRefresh);
                    printf("void highlightElement() : for {} : i = %d : if ... : RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer[elementIndex].highlighted = %d  \n",i,RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer[elementIndex].highlighted);
                    
                }
                
        }
        return 0;
    }

    void initializeRowLineCluster(int type,int clusterVerticalOffset,int clusterIndex,int bufferSize,rowLineRegion* parentRowLineRegion,int startingRowLineIndex,int numberOfRowLines,rowLineRegionElementInfo* elementPrototype,int spacing);
    void initializeRowLineCluster2(int clusterVerticalOffset,int clusterIndex,int maxNumberOfElements,rowLineRegion* parentRowLineRegion,int startingRowLineIndex,int numberOfRowLines,rowLineRegionElementInfo* elementPrototype,int elementListStartIndex,int elementListEndIndex,bool isSpecialCluster = 0, int NumberOfInsertionPoints = 0,int* InsertionPoints = 0, int* NumberOfExtraRowlinesNeededAtInsertionPoint = 0);



    int addElement2(int* uiElementIDs,char** textBoxStrings);
    int addElement3(int* uiElementIDs,char** textBoxStrings);

    void createRowLine(int rowLineBufferIndex, bool textBoxOrUiElement,int bufferSize, int layer, int pixelCoordinateY);

    int findMouseLocation(mouse* uiMouse);



    void appendRowLineToCluster(rowLineRegion* parentRowLineRegion);

    void shiftRowLines(int offset){
        for(int i = 0; i < NumberOfRowLinesInCluster; i++){
            RowLineBuffer[StartingRowLineIndex+i].PixelCoordinateY = RowLineBuffer[StartingRowLineIndex+i].PixelCoordinateY + offset;
        }

    }

    int centerElement(int elementIndex);



    //NOTE: The RowLineCluster is NOT responsible for freeing memory the memory of the RowLines this cluster is associated with
    //      Free'ing the memory of the RowLines is Strictly the responsibility of the RowLineRegion
    void freeMemory(){

        free(NumberOfRowLinesPerElement);
        free(ElementStartingX);

    }

};

//In general a rowlineregion is a typically scrollable region of space containing visuals that are
//predicatably structured, and are aligned on horizontal rows along the screen. A rowlineregion may additionally
//be structured such that its visuals are aligned into rows and columns. The latter useful for things
//like file managers, the former useful for things like a scrollable text display

//how should mixed
class rowLineRegion{
    public:
    //type 1 means vertical column, extends horizontally via scrollbar
    //type 0 means a mxn grid, does NOT extend horizontally
    int Type;



    


    int LeftSideXValue;
    int RightSideXValue;
    int TopSideYValue;
    int BottomSideYValue;

    int* VirtualLeftSideXValue;
    int* VirtualRightSideXValue;
    int* VirtualTopSideYValue;
    int* VirtualBottomSideYValue;

     //an "element" of a rowLineRegion, is the collection of rowLines that make up a cohesive visual (usually selectable) component
    //a "rowLineCluster" is the same aformentioned collection of rowLines, but not pertaining to a single "element" but the rowLines themselves that extend across the screen
    //For instance in a file manager pane's rowLineRegion, an element could consist of 3 rowlines:
    //one for the file icon, one for the text box containing the filename, and another row Line layered underneath with a color that becomes visible when the file is selected

    //When this class reads from RowLineBuffer*, we need to keep in mind that the RowLineBuffer* needs to be ordered in the following way (following our example from before)
    // RowLineBuffer[0] contains  fileIcon0, RowLineBuffer[1] contains filenameText0, RowLineBuffer[2] contains the "selected" backdrop0
    // RowLineBuffer[i] contains the fileIconi, RowLineBuffer[i+1] contains the filenameTexti, RowLineBuffer[i+2] contains the "selected" backdropi


    int NumberOfElementPrototypes;
    rowLineRegionElementInfo* ElementPrototypes; //rowLineClusters may be of different element prototypes, but elements within the same rowLineCluster must have same element prototype. To accomplish mixed element prototypes horizontally you use multiple rowLineClusters that overlap
    int* MaxNumberOfElements;                   //An array of NumberOfElementPrototypes size, wherein each entry describes the max Number of elements the rowlineregion can hold of that elementType
    int* NumberOfElements;                       //An array of NumberOfElementPrototypes size, that keeps track of how many elements of each type of elementPrototype are in the rowlineregion, EX: NumberOfElements[1] = 2 means there are 2 elements of the ElementPrototypes[1] structure in this rowlineregion
    char*** TextBoxStrings;                      //An array of an array of strings. textBoxStrings[0] is the array of strings pertaining to the list of elements of ElementPrototypes[0] type/structure
    int** visualElementID;                       //An array of an array of ints representing the visualElementID. visualElementID[0] is the array of visualElementIDS pertaining to the list of elements of ElementPrototypes[0] type/structure
    int** elementHighlights;                     //.....
    int** elementMouseSelected;
    //int*** elementIsConnectedToOtherElementType; //todo


    //These are clusters that may have textboxes that wrap around necessitating inserted rowlines, or other edge cases that break repeating structure
    //if specialElement[i] = -1, then this element is not a special element
    //if specialElement[i] = x, where x is >= 0 and < 1,000,000  THEN this cluster requires additional rowlines due to text wrap around, and accesses the specialCluster_textWrapAround_ family of arrays, the value of x represents the index that this element should use to access its specialCluster_TextWrapAroundArrays
    int** specialElement;                  
    int* NumberOfSpecialElements;
    

    int** specialElement_TextWrapAround_NumberOfTotalExtraRowlines;
    int** specialElement_TextWrapAround_NumberOfInsertionPoints; //this is the same as "NumberOfTextboxesNeedingWrapAround"
    int*** specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline;
    int*** specialElement_TextWrapAround_InsertionPoints; //this is indice of the prototype element rowline that contains the textbox needing a wrap around
    int*** specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint; //following from the above example, if a cluster has 3 rowlines, an insertionPoint at 1, and NumberOfExtraRowlinesNeededAtInsertionPoint = 2, then this means it means there has been two textbox rowlines inserted starting at index 1, so that means the orginal index 2 becomes 4, and 3 becomes 5
  







    int* NumberOfSections; //NumberOfSections[0] = 2, means this rowlineregion has two repeating sections of the 0th element type
    int* MaxNumberOfSections; //NumberOfSections[0] = x, means this rowlineregion can support x amount of repeating sections for the 0th element type
    int** RepeatingSectionsNumberOfClustersInSection;
    int** RepeatingSectionsNumberOfElementsPerCluster;
    int** RepeatingSectionsRowLineClusterAndSpacingHeight;
    //repeatingSectionsStartingPixelYCoordinates[0] corresponds to the starting y pixel coordinates of regions of space that follow the same rowlinecluster structure/pattern of type elementPrototype[0]
    // (repeatingSectionsStartingPixelYCoordinates[][numberOfSections] - 1) is the value of the ending y pixel of section (numberOfSections - 1)
    // so for example if there is 1 section that runs from y = 600 to y = 1200, then repeatingSectionsStartingPixelYCoordinates[][numberOfSections - 1] = 600 and repeatingSectionsStartingPixelYCoordinates[][numberOfSections] = 1200
    //
    int** RepeatingSectionsStartingPixelYCoordinates;


    int** RepeatingSectionIsSpecial; // RepeatingSectionIsSpecial[ElementType][repeatingSectionIndex] = TRUE/FALSE, signifies if this indexed repeatingSection is "special" 
    int* NumberOfSpecialRepeatingSections; // NumberOfSpecialRepeatingSections[ElementType]
    int** specialCluster_TotalNumberOfExtraRowLines;
    int** specialCluster_NumberOfInsertionPoints;
    int** specialCluster_TotalExtraYOffset;
    int*** specialCluster_InsertionPointYOffsetForNextRowline;
    int*** specialCluster_InsertionPoints;
    int*** specialCluster_NumberOfExtraRowlinesNeededAtInsertionPoint;


    

    

    /////////////////////////////////////////////UpdateVisibleIndices()///////////////////////////


    int FirstVisibleRowLineIndexTop; // I think these two variables will fall away completely
    int LastVisibleRowLineIndexBottom;

    bool* clusterVisibilityHasChanged;
    int* FirstVisibleRowLineClusterIndexTopDelta; // 
    int* LastVisibleRowLineClusterIndexBottomDelta;

    int* FirstVisibleRowLineClusterIndexTop;
    int* LastVisibleRowLineClusterIndexBottom;

    ////////////////////////////////////////////////////////////////////////////////////////////////





    
    
    int MaxNumberOfElementsPerCluster; //MAX number
    //int* startingElementIndexInCluster; //malloc allocated array 
    //int needsRefresh[4];
    int needsRefresh[5];

    int RowLineBufferSize;
    int RowLineBufferIndex;
    rowLineAlignment* RowLineBuffer;

    int MaxNumberOfRowLineClusters;
    
    int RowLineClusterIndex;
    int* RowLineClusterIndices; //array of indices pertaining to where each rowLineCluster starts in the RowLineBuffer
    rowLineCluster* RowLineClusters;

    //rowLineCluster* PrototypeRowlineClusters; //an array of "NumberOfElementPrototypes" size, this is used on appendElement() to check if the new element needs special considerations (extra rowlines,etc...)



    int nextRowLineClusterYOffset;



    
    //distanceToLastRowLine signifies the position offset value that would need to be added to the very last (non-visible) Row Line in order to make it visible/at bottom
    //of rowLineRegion. If the very last Row Line is already visible this value is ignored and set to -2147483648 to signal this.
    //this variable is updated using the updateVisibleIndices()
    int DistanceToLastRowLine;



    


    bool RowLinesHaveScrolled;
    



   
    //i think all of these mouse selected arrays will fall away in favor of the element list mouse selected array

    int MaxNumberOfSystemSelectedElements;
    int NumberOfSystemSelectedElements;
    int* systemSelectedLocation;

    int MaxNumberOfMouseSelectedElements;
    int NumberOfMouseSelectedElements;
    //malloc allocated array, each selected location stores two ints in the array, the first
    //being the cluster location, the second being the element location
    int* mouseSelectedLocation;
    bool isMouseMultiSelectActive = 0;

    //int NumberOfRowLinesPerElement;
    //int ElementWidth;
    //int ElementHeight;

    //spacing in between elements
    //int HorizontalSpacing;
    //int Spacing; //the vertical spacing in between clusters


    void appendElement2(int* uiElementIDs, char** textBoxStrings, int elementType = 0,bool debug = 0){
        

        
        if(debug==1){printf("DEBUG appendElement2(): elementType = %d \n",elementType);}
        if(debug==1){printf("DEBUG appendElement2(): ElementPrototypes[elementType].numberOfRowLinesPerElement = %d \n",ElementPrototypes[elementType].numberOfRowLinesPerElement);}
        int specialElementIndex = -1; //if this element is found to be one that requires a special cluster, then this is assigned the index value 
    

        //int cond = 0; //to do



        for (int i = 0, j = 0, k = 0; i < ElementPrototypes[elementType].numberOfRowLinesPerElement; i++){ // i is the rowLine
            if(debug==1){printf("DEBUG appendElement2(): i = %d, j = %d, k = %d \n", i,j,k);}

            if((ElementPrototypes[elementType].ordering[i] == 0)){
                if(debug==1){printf("\t ElementPrototypes[elementType].ordering[i] == 0 \n");}
                //add textbox content (the string) to the elementList
                int textBoxStringsIndex = ElementPrototypes[elementType].numberOfTextBoxesInElement * NumberOfElements[elementType];
                if(debug==1){printf("\t textBoxStringsIndex = %d * %d = %d \n",ElementPrototypes[elementType].numberOfTextBoxesInElement,NumberOfElements[elementType],textBoxStringsIndex);}
                TextBoxStrings[elementType][textBoxStringsIndex+j] = textBoxStrings[j];    
                if(debug==1){printf("\t TextBoxStrings[%d][%d] = %s \n",elementType,textBoxStringsIndex+j,TextBoxStrings[elementType][textBoxStringsIndex+j]);}


                if(ElementPrototypes[elementType].textBoxWrapAround[i] == 1){
                    if(debug==1){printf("\t\t ElementPrototypes[elementType].textBoxWrapAround[i] == 1 \n");}
                    int TextBoxLength = ElementPrototypes[elementType].textBoxMaxStringLength[i]; //todo
                    if(debug==1){printf("\t\t TextBoxLength = %d \n",TextBoxLength);}
                    int TextBoxMaxWidth = ElementPrototypes[elementType].textBoxMaxWidth[i]; //to do
                    if(debug==1){printf("\t\t TextBoxMaxWidth = %d \n",TextBoxMaxWidth);}

                    int nextRowlineUpperBoundY = -9999; //-9999 = INVALID_DATA_ROWLINE
                    if((i + 1 < ElementPrototypes[elementType].numberOfRowLinesPerElement) && (ElementPrototypes[elementType].ordering[i+1] == 0)){
                        if(debug==1){printf("\t\t\t (i + 1 < ElementPrototypes[elementType].numberOfRowLinesPerElement) && (ElementPrototypes[elementType].ordering[i+1] == 0) \n");}
                        nextRowlineUpperBoundY = ElementPrototypes[elementType].firstElementPixelCoordinateY + ElementPrototypes[elementType].textBoxes[i+1].RowLineAlignmentPixelCoordinateY - ElementPrototypes[elementType].textBoxes[i+1].Height;
                        if(debug==1){printf("\t\t\t nextRowlineUpperBoundY = %d + %d - %d = %d \n",ElementPrototypes[elementType].firstElementPixelCoordinateY, ElementPrototypes[elementType].textBoxes[i+1].RowLineAlignmentPixelCoordinateY,ElementPrototypes[elementType].textBoxes[i+1].Height, nextRowlineUpperBoundY);}
                    }
                    else if((i + 1 < ElementPrototypes[elementType].numberOfRowLinesPerElement) && (ElementPrototypes[elementType].ordering[i+1] == 1)){
                        if(debug==1){printf("\t\t\t (i + 1 < ElementPrototypes[elementType].numberOfRowLinesPerElement) && (ElementPrototypes[elementType].ordering[i+1] == 1) \n");}
                        nextRowlineUpperBoundY = ElementPrototypes[elementType].firstElementPixelCoordinateY + ElementPrototypes[elementType].VisualElements[i+1].RowLineAlignmentPixelCoordinateY - ElementPrototypes[elementType].VisualElements[i+1].Height;
                        if(debug==1){printf("\t\t\t nextRowlineUpperBoundY = %d + %d - %d = %d \n",ElementPrototypes[elementType].firstElementPixelCoordinateY, ElementPrototypes[elementType].VisualElements[i+1].RowLineAlignmentPixelCoordinateY, ElementPrototypes[elementType].VisualElements[i+1].Height, nextRowlineUpperBoundY);}
                    }

                    int extraRowLineY = ElementPrototypes[elementType].firstElementPixelCoordinateY + ElementPrototypes[elementType].textBoxes[i].RowLineAlignmentPixelCoordinateY; //initialize this to the rowlineY value of the textbox that is being wrapped
                    if(debug==1){printf("\t\t extraRowLineY = %d \n", extraRowLineY);}
                    int numberOfExtraRowLinesForTextBox = 0;
                    int stringSize = getStringLength(textBoxStrings[j]);
                    
                    
                    int numberOfCharactersLeft = stringSize;
                    
                    if(debug==1){printf("\t\t numberOfExtraRowLinesForTextBox = %d ,  stringSize = %d,  numberOfCharactersLeft = %d \n", numberOfExtraRowLinesForTextBox,stringSize,numberOfCharactersLeft);}
                    while(numberOfCharactersLeft > 0){
                        if(debug==1){printf("\t\t\t while(numberOfCharactersLeft > 0) \n");}
                        if(debug==1){printf("\t\t\t stringSize - numberOfCharactersLeft = %d - %d = %d \n",stringSize,numberOfCharactersLeft, (stringSize - numberOfCharactersLeft));}
                        textBox text = {};
                        numberOfCharactersLeft = text.intitializeTextBox(TextBoxLength,0,0,12,0,textBoxStrings[j] + (stringSize - numberOfCharactersLeft),numberOfCharactersLeft,TextBoxMaxWidth);
                        if(debug==1){printf("\t\t\t numberOfCharactersLeft = %d \n", numberOfCharactersLeft);}
                        if(numberOfCharactersLeft > 0){
                            if(debug==1){printf("\t\t\t\t if(numberOfCharactersLeft > 0) \n");}

                            if(nextRowlineUpperBoundY != -9999){
                                if(debug==1){printf("\t\t\t\t\t if(nextRowlineUpperBoundY != -9999) \n");}
                                extraRowLineY = extraRowLineY + ElementPrototypes[elementType].textBoxes[i].Height + ElementPrototypes[elementType].textBoxWrapAroundSpacing[i]; //CHANGE THIS TO WRAPPING VERTICAL SPACING
                                if(debug==1){printf("\t\t\t\t\t extraRowLineY = %d \n",extraRowLineY);}
                                if(extraRowLineY > nextRowlineUpperBoundY){
                                    if(debug==1){printf("\t\t\t\t\t\t if(extraRowLineY > nextRowlineUpperBoundY) \n");}
                                    specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[elementType][NumberOfSpecialElements[elementType]][specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][NumberOfSpecialElements[elementType]]] = extraRowLineY - nextRowlineUpperBoundY;
                                    if(debug==1){printf("\t\t\t\t\t\t specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[%d][%d][%d] = %d - %d = %d \n",elementType,NumberOfSpecialElements[elementType], specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][NumberOfSpecialElements[elementType]],extraRowLineY,nextRowlineUpperBoundY,specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[elementType][NumberOfSpecialElements[elementType]][specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][NumberOfSpecialElements[elementType]]]);}
                                }
                            }
                            
                            
                            if(numberOfExtraRowLinesForTextBox == 0){
                                if(debug==1){printf("\t\t\t\t\t if(numberOfExtraRowLinesForTextBox == 0) \n");}
                                specialElement_TextWrapAround_InsertionPoints[elementType][NumberOfSpecialElements[elementType]][specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][NumberOfSpecialElements[elementType]]] = i;  
                                if(debug==1){printf("\t\t\t\t\t specialElement_TextWrapAround_InsertionPoints[%d][%d][%d] = %d \n", elementType, NumberOfSpecialElements[elementType], specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][NumberOfSpecialElements[elementType]], i);}
                            }
            


                            specialElement_TextWrapAround_NumberOfTotalExtraRowlines[elementType][NumberOfSpecialElements[elementType]] = specialElement_TextWrapAround_NumberOfTotalExtraRowlines[elementType][NumberOfSpecialElements[elementType]] + 1;
                            if(debug==1){printf("\t\t\t\t specialElement_TextWrapAround_NumberOfTotalExtraRowlines[elementType][NumberOfSpecialElements[elementType]] = %d \n", specialElement_TextWrapAround_NumberOfTotalExtraRowlines[elementType][NumberOfSpecialElements[elementType]]);}
                            numberOfExtraRowLinesForTextBox = numberOfExtraRowLinesForTextBox + 1;
                            if(debug==1){printf("\t\t\t\t numberOfExtraRowLinesForTextBox = %d \n", numberOfExtraRowLinesForTextBox);}
                        }

                    }
                    if(debug==1){printf("\t\t EXITED WIHLE LOOP \n");}
                    if(numberOfExtraRowLinesForTextBox > 0){
                        if(debug==1){printf("\t\t\t if(numberOfExtraRowLinesForTextBox > 0) \n");}
                        specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][NumberOfSpecialElements[elementType]][specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][NumberOfSpecialElements[elementType]]] = numberOfExtraRowLinesForTextBox;
                        if(debug==1){printf("\t\t\t specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[%d][%d][%d] = %d \n",elementType, NumberOfSpecialElements[elementType], specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][NumberOfSpecialElements[elementType]],numberOfExtraRowLinesForTextBox);}
                        specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][NumberOfSpecialElements[elementType]] = specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][NumberOfSpecialElements[elementType]] + 1;
                        if(debug==1){printf("\t\t\t specialElement_TextWrapAround_NumberOfInsertionPoints[%d][%d] = %d \n",elementType,NumberOfSpecialElements[elementType], specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][NumberOfSpecialElements[elementType]]);}
                        if(specialElementIndex == -1){
                            if(debug==1){printf("\t\t\t\t if(specialElementIndex == -1) \n");}
                            specialElementIndex = NumberOfSpecialElements[elementType];
                            if(debug==1){printf("\t\t\t\t specialElementIndex = %d \n",specialElementIndex);}
                        }
                    }
                        
                    
                }

                j = j + 1;
                if(debug==1){printf("\t j = %d \n", j);}
            }

            if((ElementPrototypes[elementType].ordering[i] == 1)){
                if(debug==1){printf("\t (ElementPrototypes[elementType].ordering[i] == 1) \n");}
                //add visualElement content (the visualElementID) to the elementList
                int visualElementIDIndex = ElementPrototypes[elementType].numberOfVisualElementsInElement * NumberOfElements[elementType];
                if(debug==1){printf("\t visualElementIDIndex = %d * %d = %d \n",ElementPrototypes[elementType].numberOfVisualElementsInElement,NumberOfElements[elementType], visualElementIDIndex);}
                visualElementID[elementType][visualElementIDIndex+k] = uiElementIDs[k];
                if(debug==1){printf("\t visualElementID[%d][%d] = %d \n",elementType,visualElementIDIndex+k,visualElementID[elementType][visualElementIDIndex+k]);}
                k = k + 1;
                if(debug==1){printf("\t k = k +1 = %d \n", k);}
                
            }
            
        }

        specialElement[elementType][NumberOfElements[elementType]] = -1;
        if(specialElementIndex != -1){
            if(debug==1){printf("if(specialElementIndex != -1) \n");}
            specialElement[elementType][NumberOfElements[elementType]] = specialElementIndex;
            if(debug==1){printf("specialElement[%d][%d] = %d \n",elementType,NumberOfElements[elementType], specialElement[elementType][NumberOfElements[elementType]]);}
            NumberOfSpecialElements[elementType] = NumberOfSpecialElements[elementType] + 1;
            if(debug==1){printf("NumberOfSpecialElements[%d] = %d \n",elementType,NumberOfSpecialElements[elementType]);}
        }

        NumberOfElements[elementType] = NumberOfElements[elementType] + 1;
        if(debug==1){printf("NumberOfElements[%d] = %d \n",elementType,NumberOfElements[elementType]);}
    }



    //if elementIndex is negative:
    //  -1 indicates elementPrototype[0]
    //  -2 indciates elementPrototype[1]
    //....
    int getElementWidth(int elementIndex){


    }

    int buildRepeatingSections(int elementType){
        if(Type == 0){
            //EDIT:05/03/2025, I think this should include the elementPrototype.horizontalSpacing, so i added it
            //EDIT:05/03/2025, I think ElementPrototype->firstElementPixelCoordinateX will become an offset value of the rowlineregion x bounds, so now we will add that bound value to the offset
            int prototypeElementWidth = getElementWidth((-elementType) - 1);
            int numberOfElementsPerCluster = 0; 
            int elementStartingXPosition = ElementPrototypes[elementType].firstElementPixelCoordinateX; // + LeftSideXValue
            while(elementStartingXPosition + prototypeElementWidth + ElementPrototypes[elementType].horizontalSpacing <= RightSideXValue){
                numberOfElementsPerCluster = numberOfElementsPerCluster + 1;
                elementStartingXPosition = elementStartingXPosition + prototypeElementWidth + ElementPrototypes[elementType].horizontalSpacing;

            }

            if(numberOfElementsPerCluster <= 0 ){
                return -1;
            }


            bool isRowLineInsertionPoints[2][ElementPrototypes[elementType].numberOfRowLinesPerElement];
            int insertionPoints_MaxYOffsetForNextRowline[2][ElementPrototypes[elementType].numberOfRowLinesPerElement];
            int insertionPoints_MaxNumerOfExtraRowLinesNeeded[2][ElementPrototypes[elementType].numberOfRowLinesPerElement];




            //if we encounter an element that requires a special rowlinecluster due to a textbox wrap around
                    
            //so say we are 64 elements into a list of 332 elements and the 65th element is a special element meaning this is a special cluster and the start of a specialRepeatingSection,
            // the numberOfElementsPerCluster is 7
            // 64 / 7 = 9.14 -> 9
            
            //This tells us that 9 * 7 = 63, and thus 63 is the index of the last element that falls in section 0, and index 64 is the first element that falls in section 1
            //This also tells us that there are 9 clusters in section 0

            // 63 is the cutoff index for section 0, and since there is 7 elements to a cluster, Any element from indexes 64 to 70 inclusive will be a part of this special cluster
            //


            //so say we are 64 elements into a list of 332 elements and the 65th element is a special element meaning this is a special cluster and the start of a specialRepeatingSection,
            // the numberOfElementsPerCluster is 11
            // 64 / 11 = 5.xx -> 5
            
            //This tells us that 5 * 11 = 55, and thus 55 is the index of the last element that falls in section 0, and index 56 is the first element that falls in section 1
            //This also tells us that there are 5 clusters in section 0

            // 63 is the cutoff index for section 0, and since there is 7 elements to a cluster, Any element from indexes 64 to 70 inclusive will be a part of this special cluster
            //


            /*once the for(int j...){..} loop concludes i will be incremented */

            NumberOfSections[elementType] = 0;//NumberOfElements[elementType] > 0;
            
            int currentSection = 0;
            int numberOfElementsInSection = 0;
            int normalElementRunCount = 0;
            //bool wasLastClusterSpecial = 0;
            //bool isCurrentClusterSpecial = 0;
            bool isCurrentSectionSpecial = 0;
            bool hasCompletedANormalCluster = 0;
            for(int i =0; i < NumberOfElements[elementType]; i++){
                //A: if((specialElement[elementType][i] > 0 && specialElement[elementType][i] < 1'000'000) && (hasCompletedANormalCluster == 1 && isCurrentSectionSpecial == 0)) THEN we are marking the end of a normal section the start of a special section. 
                //B: if((specialElement[elementType][i] > 0 && specialElement[elementType][i] < 1'000'000) && (hasCompletedANormalCluster == 0 && isCurrentSectionSpecial == 1)){} Then this means we need to handle a new special cluster, and, since we are currently already in a special section, we need to see if the new special cluster matches the special cluster that pertains to this special section, if so continue the special section, if not record the special section and then start a new one
                //C: if((specialElement[elementType][i] > 0 && specialElement[elementType][i] < 1'000'000) && (hasCompletedANormalCluster == 0 && isCurrentSectionSpecial == 0)){} Then we only mark the start of a special section, since we never had a normal cluster. NOTE: This can/should ONLY happen when creating the first section and the first section is special
                //D: else{if((hasCompletedANormalCluster == 1 && isCurrentSectionSpecial == 1))} Then this means we are EITHER simply incrementing the numberOfElementsInSection if this section is normal, if this section is special then if this branch scans a whole cluster that is normal it must mark the end of the special section and the start of a normal one
                

                //Path A: increments currentSection, resets numberOfElementsInSection to 0 before then setting it to the amount of elements in the detected special cluster, sets isCurrentSectionSpecial to 1, resets hasCompletedANormalCluster back to 0
                //Path B0 (this is the path where the detected special cluster matches the previous one, and continues the section): increments currentSection, numberOfElementsInSection is added onto/accumulated with the amount of elements in the detected special cluster, isCurrentSectionSpecial is left as 1, hasCompletedANormalCluster is left as 0
                //Path B1 (this is the path where the detected special cluster does NOT match the previous one, and creates a new special section)
                //Path C: currentSection is left at 0, numberOfElementsInSection is set to the number of elements in the detected special cluster, isCurrentSectionSpecial is set to 1, hasANormalCluster is left at 0
                //Path D0: increments numberOfElementsInSection
                //Path D1: increments currentSection, it first decrements numberOfElementsInSection by the amount of elements in a cluster (this is so we dont add the detected normal cluster to the special section we are ending) then once we are done ending the special section and starting the new normal section we set numberOfElementsInSection to the number of elements in a cluster, isCurrentSecionSpecial is set to 0


                //Path A:
                if((specialElement[elementType][i] > 0 && specialElement[elementType][i] < 1'000'000) && ((hasCompletedANormalCluster == 1) && (isCurrentSectionSpecial == 0) )){
                    normalElementRunCount = 0;

                    int currentSpecialClusterStartIndex = (i / numberOfElementsPerCluster) * numberOfElementsPerCluster;
                    int currentSpecialClusterEndIndex = returnMin(currentSpecialClusterStartIndex + numberOfElementsPerCluster,NumberOfElements[elementType]);

                    

                    ///////////////////////mark the end of the collected normal section and the start of the special section//////////////////////////
                    int intoCluster = numberOfElementsInSection % numberOfElementsPerCluster;
                    numberOfElementsInSection = numberOfElementsInSection - intoCluster;

                    NumberOfSections[elementType] = NumberOfSections[elementType] + 1;
                    RepeatingSectionsNumberOfElementsPerCluster[elementType][currentSection] = numberOfElementsPerCluster;
                    RepeatingSectionsNumberOfClustersInSection[elementType][currentSection] = ((numberOfElementsInSection-1) / RepeatingSectionsNumberOfElementsPerCluster[elementType][currentSection]) + 1;
                    RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection] = 0;
                    if(currentSection > 0){
                        RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection] = RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection-1] + (RepeatingSectionsNumberOfClustersInSection[elementType][currentSection-1] * RepeatingSectionsRowLineClusterAndSpacingHeight[elementType][currentSection-1]);
                    }
                    numberOfElementsInSection = 0;
                    
                    hasCompletedANormalCluster = 0; 

                    currentSection = currentSection + 1;
                    RepeatingSectionIsSpecial[elementType][currentSection] = NumberOfSpecialRepeatingSections[elementType];//currentSection;    

                    isCurrentSectionSpecial = 1;     
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////       
                    
                    
                    
                    
                    
                    
                    memset(isRowLineInsertionPoints[0],0,sizeof(bool) * ElementPrototypes[elementType].numberOfRowLinesPerElement);
                    memset(insertionPoints_MaxYOffsetForNextRowline[0],0, sizeof(int) * ElementPrototypes[elementType].numberOfRowLinesPerElement);
                    memset(insertionPoints_MaxNumerOfExtraRowLinesNeeded[0],0, sizeof(int) * ElementPrototypes[elementType].numberOfRowLinesPerElement);

                    int j = currentSpecialClusterStartIndex;
                    for(j; j < currentSpecialClusterEndIndex; j++){
                        int specialElementIndex = specialElement[elementType][j];
                        if(specialElementIndex > 0 && specialElementIndex < 1'000'000){

                            for(int k = 0; k < specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][specialElementIndex]; k++){
                                int insertionPointsIndex = specialElement_TextWrapAround_InsertionPoints[elementType][specialElementIndex][k]; 
                                if(isRowLineInsertionPoints[0][insertionPointsIndex] == 0){isRowLineInsertionPoints[0][insertionPointsIndex] = 1;}

                                if(specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[elementType][specialElementIndex][k] > insertionPoints_MaxYOffsetForNextRowline[0][insertionPointsIndex]){
                                    insertionPoints_MaxYOffsetForNextRowline[0][insertionPointsIndex] = specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[elementType][specialElementIndex][k];
                                }
                                if(specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][specialElementIndex][k] > insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][insertionPointsIndex]){
                                    insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][insertionPointsIndex] = specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][specialElementIndex][k];
                                }

                            }
                        }
                        

                    }
                    



                    int numberOfInsertionPoints = 0;
                    int accumlatedYOffset = 0;
                    int accumulatedNumberOfExtraRowlines = 0;
                    for(int k=0; k < ElementPrototypes[elementType].numberOfRowLinesPerElement; k++){
                       
                        if(isRowLineInsertionPoints[0][k]){
                            specialCluster_InsertionPoints[elementType][NumberOfSpecialRepeatingSections[elementType]][numberOfInsertionPoints] = k;
                            specialCluster_InsertionPointYOffsetForNextRowline[elementType][NumberOfSpecialRepeatingSections[elementType]][numberOfInsertionPoints] = insertionPoints_MaxYOffsetForNextRowline[0][k];
                            specialCluster_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][NumberOfSpecialRepeatingSections[elementType]][numberOfInsertionPoints] = insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][k];
                            
                            accumulatedNumberOfExtraRowlines = accumulatedNumberOfExtraRowlines + insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][k];
                            accumlatedYOffset = accumlatedYOffset + insertionPoints_MaxYOffsetForNextRowline[0][k];
                            numberOfInsertionPoints = numberOfInsertionPoints + 1;
                        }
                    }
                    specialCluster_TotalExtraYOffset[elementType][NumberOfSpecialRepeatingSections[elementType]] = accumlatedYOffset;
                    specialCluster_NumberOfInsertionPoints[elementType][NumberOfSpecialRepeatingSections[elementType]] = numberOfInsertionPoints;
                    specialCluster_TotalNumberOfExtraRowLines[elementType][NumberOfSpecialRepeatingSections[elementType]] = accumulatedNumberOfExtraRowlines;

                    NumberOfSpecialRepeatingSections[elementType] = NumberOfSpecialRepeatingSections[elementType] + 1;
                    

                    numberOfElementsInSection =  currentSpecialClusterEndIndex - currentSpecialClusterStartIndex;
                    i = j -1;
                }



                //Path B:
                else if((specialElement[elementType][i] > 0 && specialElement[elementType][i] < 1'000'000) && ((hasCompletedANormalCluster == 0) && (isCurrentSectionSpecial == 1) )){
                    //first iterate through the detected special cluster
                    //
                    normalElementRunCount = 0;
                    
                    int currentSpecialClusterStartIndex = (i / numberOfElementsPerCluster) * numberOfElementsPerCluster;
                    int currentSpecialClusterEndIndex = returnMin(currentSpecialClusterStartIndex + numberOfElementsPerCluster,NumberOfElements[elementType]);

                    memset(isRowLineInsertionPoints[1],0,sizeof(bool) * ElementPrototypes[elementType].numberOfRowLinesPerElement);
                    memset(insertionPoints_MaxYOffsetForNextRowline[1],0, sizeof(int) * ElementPrototypes[elementType].numberOfRowLinesPerElement);
                    memset(insertionPoints_MaxNumerOfExtraRowLinesNeeded[1],0, sizeof(int) * ElementPrototypes[elementType].numberOfRowLinesPerElement);

                    int j = currentSpecialClusterStartIndex;
                    for(j; j < currentSpecialClusterEndIndex; j++){
                        int specialElementIndex = specialElement[elementType][j];
                        if(specialElementIndex > 0 && specialElementIndex < 1'000'000){
                            for(int k = 0; k < specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][specialElementIndex]; k++){
                                int insertionPointsIndex = specialElement_TextWrapAround_InsertionPoints[elementType][specialElementIndex][k]; 
                                if(isRowLineInsertionPoints[1][insertionPointsIndex] == 0){isRowLineInsertionPoints[1][insertionPointsIndex] = 1;}

                                if(specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[elementType][specialElementIndex][k] > insertionPoints_MaxYOffsetForNextRowline[1][insertionPointsIndex]){
                                    insertionPoints_MaxYOffsetForNextRowline[1][insertionPointsIndex] = specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[elementType][specialElementIndex][k];
                                }
                                if(specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][specialElementIndex][k] > insertionPoints_MaxNumerOfExtraRowLinesNeeded[1][insertionPointsIndex]){
                                    insertionPoints_MaxNumerOfExtraRowLinesNeeded[1][insertionPointsIndex] = specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][specialElementIndex][k];
                                }

                            }
                        }
                    }

                    //see if this new detected special cluster matches the previous detected special cluster
                    bool detectedSpecialClusterMatchesLastSpecialCluster = 1; //assume true until/unless proven false by for loop
                    for(int j = 0; j < ElementPrototypes[elementType].numberOfRowLinesPerElement; j++){
                        if(!((isRowLineInsertionPoints[0][j] == isRowLineInsertionPoints[1][j]) && (insertionPoints_MaxYOffsetForNextRowline[0][j] == insertionPoints_MaxYOffsetForNextRowline[1][j]) && (insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][j] == insertionPoints_MaxNumerOfExtraRowLinesNeeded[1][j]))){
                            detectedSpecialClusterMatchesLastSpecialCluster = 0;
                        }

                    }

                    if(detectedSpecialClusterMatchesLastSpecialCluster == 0){
                        //Mark the end of the last special section and the start of the current one
                        NumberOfSections[elementType] = NumberOfSections[elementType] + 1;
                        RepeatingSectionsNumberOfElementsPerCluster[elementType][currentSection] = numberOfElementsPerCluster;
                        RepeatingSectionsNumberOfClustersInSection[elementType][currentSection] = ((numberOfElementsInSection-1) / RepeatingSectionsNumberOfElementsPerCluster[elementType][currentSection]) + 1;

                        RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection] = RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection-1] + (RepeatingSectionsNumberOfClustersInSection[elementType][currentSection-1] * (RepeatingSectionsRowLineClusterAndSpacingHeight[elementType][currentSection-1] + specialCluster_TotalExtraYOffset[elementType][(NumberOfSpecialRepeatingSections[elementType] - 1)])); 
                        
                        numberOfElementsInSection = 0;
                         

                        currentSection = currentSection + 1;
                        RepeatingSectionIsSpecial[elementType][currentSection] = NumberOfSpecialRepeatingSections[elementType];;    

                        isCurrentSectionSpecial = 1; 
                        ////////////////////////////////////////////////////////////////////////////////////////////// 
                        //memcpy the [1] stores into the [0] stores
                        //memset the [1] stores to 0
                        //and then record the new special cluster
                        memcpy(isRowLineInsertionPoints[0],isRowLineInsertionPoints[1],ElementPrototypes[elementType].numberOfRowLinesPerElement * sizeof(bool));
                        memcpy(insertionPoints_MaxYOffsetForNextRowline[0], insertionPoints_MaxYOffsetForNextRowline[1], ElementPrototypes[elementType].numberOfRowLinesPerElement * sizeof(int));
                        memcpy(insertionPoints_MaxNumerOfExtraRowLinesNeeded[0],insertionPoints_MaxNumerOfExtraRowLinesNeeded[1],ElementPrototypes[elementType].numberOfRowLinesPerElement * sizeof(int));
                        
                        memset(isRowLineInsertionPoints[1],0,ElementPrototypes[elementType].numberOfRowLinesPerElement * sizeof(bool));
                        memset(insertionPoints_MaxYOffsetForNextRowline[1], 0, ElementPrototypes[elementType].numberOfRowLinesPerElement * sizeof(int));
                        memset(insertionPoints_MaxNumerOfExtraRowLinesNeeded[1], 0, ElementPrototypes[elementType].numberOfRowLinesPerElement * sizeof(int));
                        

                        int numberOfInsertionPoints = 0;
                        int accumlatedYOffset = 0;
                        int accumulatedNumberOfExtraRowlines = 0;
                        for(int k=0; k < ElementPrototypes[elementType].numberOfRowLinesPerElement; k++){
                        
                            if(isRowLineInsertionPoints[0][k]){
                                specialCluster_InsertionPoints[elementType][NumberOfSpecialRepeatingSections[elementType]][numberOfInsertionPoints] = k;
                                specialCluster_InsertionPointYOffsetForNextRowline[elementType][NumberOfSpecialRepeatingSections[elementType]][numberOfInsertionPoints] = insertionPoints_MaxYOffsetForNextRowline[0][k];
                                specialCluster_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][NumberOfSpecialRepeatingSections[elementType]][numberOfInsertionPoints] = insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][k];
                                
                                accumulatedNumberOfExtraRowlines = accumulatedNumberOfExtraRowlines + insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][k];
                                accumlatedYOffset = accumlatedYOffset + insertionPoints_MaxYOffsetForNextRowline[0][k];
                                numberOfInsertionPoints = numberOfInsertionPoints + 1;
                            }
                        }
                        specialCluster_TotalExtraYOffset[elementType][NumberOfSpecialRepeatingSections[elementType]] = accumlatedYOffset;
                        specialCluster_NumberOfInsertionPoints[elementType][NumberOfSpecialRepeatingSections[elementType]] = numberOfInsertionPoints;
                        specialCluster_TotalNumberOfExtraRowLines[elementType][NumberOfSpecialRepeatingSections[elementType]] = accumulatedNumberOfExtraRowlines;

                        NumberOfSpecialRepeatingSections[elementType] = NumberOfSpecialRepeatingSections[elementType] + 1;

                    }


                    //if we are continuing the special section then 
                    numberOfElementsInSection = numberOfElementsInSection + (currentSpecialClusterEndIndex - currentSpecialClusterStartIndex);
                    i = j -1;
                   

                }


                //Path C:
                else if((specialElement[elementType][i] > 0 && specialElement[elementType][i] < 1'000'000) && (hasCompletedANormalCluster == 0 && isCurrentSectionSpecial == 0)){
                    normalElementRunCount = 0;
                    
                    int currentSpecialClusterStartIndex = (i / numberOfElementsPerCluster) * numberOfElementsPerCluster;
                    int currentSpecialClusterEndIndex = returnMin(currentSpecialClusterStartIndex + numberOfElementsPerCluster,NumberOfElements[elementType]);
                    
                    
                    
                    memset(isRowLineInsertionPoints[0],0,sizeof(bool) * ElementPrototypes[elementType].numberOfRowLinesPerElement);
                    memset(insertionPoints_MaxYOffsetForNextRowline[0],0, sizeof(int) * ElementPrototypes[elementType].numberOfRowLinesPerElement);
                    memset(insertionPoints_MaxNumerOfExtraRowLinesNeeded[0],0, sizeof(int) * ElementPrototypes[elementType].numberOfRowLinesPerElement);

                    int j = currentSpecialClusterStartIndex;
                    for(j; j < currentSpecialClusterEndIndex; j++){
                        int specialElementIndex = specialElement[elementType][j];
                        if(specialElementIndex > 0 && specialElementIndex < 1'000'000){

                            for(int k = 0; k < specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][specialElementIndex]; k++){
                                int insertionPointsIndex = specialElement_TextWrapAround_InsertionPoints[elementType][specialElementIndex][k]; 
                                if(isRowLineInsertionPoints[0][insertionPointsIndex] == 0){isRowLineInsertionPoints[0][insertionPointsIndex] = 1;}

                                if(specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[elementType][specialElementIndex][k] > insertionPoints_MaxYOffsetForNextRowline[0][insertionPointsIndex]){
                                    insertionPoints_MaxYOffsetForNextRowline[0][insertionPointsIndex] = specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[elementType][specialElementIndex][k];
                                }
                                if(specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][specialElementIndex][k] > insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][insertionPointsIndex]){
                                    insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][insertionPointsIndex] = specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][specialElementIndex][k];
                                }

                            }
                        }
                        

                    }
                    



                    int numberOfInsertionPoints = 0;
                    int accumlatedYOffset = 0;
                    int accumulatedNumberOfExtraRowlines = 0;
                    for(int k=0; k < ElementPrototypes[elementType].numberOfRowLinesPerElement; k++){
                       
                        if(isRowLineInsertionPoints[0][k]){
                            specialCluster_InsertionPoints[elementType][NumberOfSpecialRepeatingSections[elementType]][numberOfInsertionPoints] = k;
                            specialCluster_InsertionPointYOffsetForNextRowline[elementType][NumberOfSpecialRepeatingSections[elementType]][numberOfInsertionPoints] = insertionPoints_MaxYOffsetForNextRowline[0][k];
                            specialCluster_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][NumberOfSpecialRepeatingSections[elementType]][numberOfInsertionPoints] = insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][k];
                            
                            accumulatedNumberOfExtraRowlines = accumulatedNumberOfExtraRowlines + insertionPoints_MaxNumerOfExtraRowLinesNeeded[0][k];
                            accumlatedYOffset = accumlatedYOffset + insertionPoints_MaxYOffsetForNextRowline[0][k];
                            numberOfInsertionPoints = numberOfInsertionPoints + 1;
                        }
                    }
                    specialCluster_TotalExtraYOffset[elementType][NumberOfSpecialRepeatingSections[elementType]] = accumlatedYOffset;
                    specialCluster_NumberOfInsertionPoints[elementType][NumberOfSpecialRepeatingSections[elementType]] = numberOfInsertionPoints;
                    specialCluster_TotalNumberOfExtraRowLines[elementType][NumberOfSpecialRepeatingSections[elementType]] = accumulatedNumberOfExtraRowlines;


                    RepeatingSectionIsSpecial[elementType][currentSection] = NumberOfSpecialRepeatingSections[elementType];
                    NumberOfSpecialRepeatingSections[elementType] = NumberOfSpecialRepeatingSections[elementType] + 1;
                    

                    isCurrentSectionSpecial = 1;
                    numberOfElementsInSection =  currentSpecialClusterEndIndex - currentSpecialClusterStartIndex;
                    i = j -1;
                }
                

                //PATH D
                else{
                    //D0
                    numberOfElementsInSection = numberOfElementsInSection + 1;
                    normalElementRunCount = normalElementRunCount + 1;
                    
                    if(normalElementRunCount == numberOfElementsPerCluster){
                        hasCompletedANormalCluster = 1;
                    }
                    
                    //Path D1
                    if((hasCompletedANormalCluster == 1 && isCurrentSectionSpecial == 1)){
                        //mark the end of special section and the start of normal section
                        //Mark the end of the last special section and the start of the current one
                        numberOfElementsInSection = numberOfElementsInSection - numberOfElementsPerCluster; //remove the elements from the discovered normal cluster from being counted towards special cluster
                        NumberOfSections[elementType] = NumberOfSections[elementType] + 1;
                        RepeatingSectionsNumberOfElementsPerCluster[elementType][currentSection] = numberOfElementsPerCluster;
                        RepeatingSectionsNumberOfClustersInSection[elementType][currentSection] = ((numberOfElementsInSection-1) / RepeatingSectionsNumberOfElementsPerCluster[elementType][currentSection]) + 1;

                        RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection] = RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection-1] + (RepeatingSectionsNumberOfClustersInSection[elementType][currentSection-1] * (RepeatingSectionsRowLineClusterAndSpacingHeight[elementType][currentSection-1] + specialCluster_TotalExtraYOffset[elementType][(NumberOfSpecialRepeatingSections[elementType] - 1)])); 
                        


                        numberOfElementsInSection = numberOfElementsPerCluster; 
                        normalElementRunCount = numberOfElementsPerCluster;
                        isCurrentSectionSpecial = 0;

                        currentSection = currentSection + 1;
                        
                        
                        //////////////////////////////////////////
                    

                    }
                }

                

            }

            if(isCurrentSectionSpecial == 0 && numberOfElementsInSection > 0){
                ///////////////////////mark the end of the collected normal section//////////////////////////
                    NumberOfSections[elementType] = NumberOfSections[elementType] + 1;
                    RepeatingSectionsNumberOfElementsPerCluster[elementType][currentSection] = numberOfElementsPerCluster;
                    RepeatingSectionsNumberOfClustersInSection[elementType][currentSection] = ((numberOfElementsInSection-1) / RepeatingSectionsNumberOfElementsPerCluster[elementType][currentSection]) + 1;
                    RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection] = 0;
                    if(currentSection > 0){
                        RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection] = RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection-1] + (RepeatingSectionsNumberOfClustersInSection[elementType][currentSection-1] * RepeatingSectionsRowLineClusterAndSpacingHeight[elementType][currentSection-1]);
                    }
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            }
            else if(isCurrentSectionSpecial == 1 && numberOfElementsInSection > 0){
                    //Mark the end of the last special section and the start of the current one
                    NumberOfSections[elementType] = NumberOfSections[elementType] + 1;
                    RepeatingSectionsNumberOfElementsPerCluster[elementType][currentSection] = numberOfElementsPerCluster;
                    RepeatingSectionsNumberOfClustersInSection[elementType][currentSection] = ((numberOfElementsInSection-1) / RepeatingSectionsNumberOfElementsPerCluster[elementType][currentSection]) + 1;

                    RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection] = 0;
                    if(currentSection > 0){
                        RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection] = RepeatingSectionsStartingPixelYCoordinates[elementType][currentSection-1] + (RepeatingSectionsNumberOfClustersInSection[elementType][currentSection-1] * (RepeatingSectionsRowLineClusterAndSpacingHeight[elementType][currentSection-1] + specialCluster_TotalExtraYOffset[elementType][(NumberOfSpecialRepeatingSections[elementType] - 1)])); 
                    }
                    ////////////////////////////////////////////////////////////////////////////////////////////// 
            }


            //VALIDATE RepeatingSections
            if(NumberOfSections[elementType] < 0){return -1;}
            for(int i = 0; i < NumberOfSections[elementType]; i++){
                if(RepeatingSectionsNumberOfElementsPerCluster[elementType][i] <= 0){return -2;}
                if(RepeatingSectionsNumberOfClustersInSection[elementType][i] <= 0){return -3;}
                if(RepeatingSectionsRowLineClusterAndSpacingHeight[elementType][i] <= 0){return -4;}


            }
            





        }

    }










    //raw top 500
    //raw bottom 1000

    // virtualTop = 1142
    //virtualBottom = 1642

    //Sections:
    //[0: 500-959]    //each element height + spacing is 115, so there are 4 clusters in this section
    //[1: 960-1499]   //each element height + spacing is 180, so there are 3 rowlineclusters
    //[2: 1500-2400]  //each element height + spacing 150, so there is 6 rowlineclusters

    //virtual top falls in section 1
    //virtual bottom falls in section 2



    
    // virtualTopSectionFirstVisibleClusterIndex = (1142-960) / 180 = 1.01 --------------> 1
    // virtualBottomSectionLastVisibleClusterIndex = (1642-1500) / 150 =0.94 -------------> 0

    //virtual top falls in section 1 and falls within cluster index 1 of that first section
    //virtual bottom falls in section 2 and falls within cluster index 0 of that second section
    //(the zeroth section isnt visible)


    
    
    //FirstVisibleRowLineCluster = numberOfClustersFromSectionsNotVisible + virtualTopSectionFirstVisibleClusterIndex = 4 + 1 = 5
    //LastVisibleRowLineCluster = numberOfClustersFromSectionsNotVisible + virtualTopSectionNumberOfClusters + middleSections + virtualBottomSectionLastVisibleClusterIndex
    //                          = 4 + 3 + 0 = 7




    
    
    //updateVisibleIndices() becomes a function that checks top/bottom virtual borders pixelYCoordinate values
    //if the virtual border's pixelYCoordinate value is still within a repeatingsection[][i] to repeatingsection[][i+1] range
    //then it performs VirtualTopSideYValue

    //on scroll down for instance, the 


    //okay what exactly are the responsiblities of this?
    //Okay remember that if the LastVisibleRowLineClusterIndexBottom[elementPrototype] < FirstVisibleRowLineClusterIndexTop[i], THEN this means the rowlineregion is EMPTY of this element
    //                   if the LastVisibleRowLineClusterIndexBottom[elementPrototype] == FirstVisibleRowLineClusterIndexTop[i], THEN this means the rowlineregion has 1 repeating sections (possibly many clusters or just one)

    int updateVisibleIndices2(){
        

        


        int previousFirstVisibleRowLineClusterIndexTop[NumberOfElementPrototypes];
        int previousLastVisibleRowLineClusterIndexBottom[NumberOfElementPrototypes];

        int virtualTopSectionInterval[NumberOfElementPrototypes];
        int virtualBottomSectionInterval[NumberOfElementPrototypes];

        int virtualTopSectionFirstVisibleClusterIndex[NumberOfElementPrototypes];
        int virtualBottomSectionLastVisibleClusterIndex[NumberOfElementPrototypes];

        for(int i = 0; i < NumberOfElementPrototypes; i++){
            if(VirtualTopSideYValue[i] > VirtualBottomSideYValue[i]){return -1;}
            previousFirstVisibleRowLineClusterIndexTop[i] = FirstVisibleRowLineClusterIndexTop[i];
            previousLastVisibleRowLineClusterIndexBottom[i] = LastVisibleRowLineClusterIndexBottom[i];
            
            if(NumberOfSections[i] <= 0){
                FirstVisibleRowLineClusterIndexTop[i] = 0;
                LastVisibleRowLineClusterIndexBottom[i] = -1;

            }
            else{
                virtualTopSectionInterval[i] = binaryIntervalSearch(RepeatingSectionsStartingPixelYCoordinates[i],NumberOfSections[i],VirtualTopSideYValue[i]);
                virtualBottomSectionInterval[i] = binaryIntervalSearch(RepeatingSectionsStartingPixelYCoordinates[i],NumberOfSections[i],VirtualBottomSideYValue[i]);
                if(virtualTopSectionInterval[i] < 0 || virtualBottomSectionInterval[i] < 0 ){return -2; }
                if(virtualTopSectionInterval[i] > virtualBottomSectionInterval[i]){return -3;}
                
                

                virtualTopSectionFirstVisibleClusterIndex[i] = (VirtualTopSideYValue[i] - RepeatingSectionsStartingPixelYCoordinates[i][virtualTopSectionInterval[i]]) / RepeatingSectionsRowLineClusterAndSpacingHeight[i][virtualTopSectionInterval[i]];
                virtualBottomSectionLastVisibleClusterIndex[i] = (VirtualBottomSideYValue[i] - RepeatingSectionsStartingPixelYCoordinates[i][virtualBottomSectionInterval[i]]) / RepeatingSectionsRowLineClusterAndSpacingHeight[i][virtualBottomSectionInterval[i]];
                if((virtualTopSectionFirstVisibleClusterIndex[i] < 0) || (virtualTopSectionFirstVisibleClusterIndex[i] >= RepeatingSectionsNumberOfClustersInSection[i][virtualTopSectionInterval[i]])){return -4;}
                if(virtualBottomSectionLastVisibleClusterIndex[i] < 0 || (virtualBottomSectionLastVisibleClusterIndex[i] >= RepeatingSectionsNumberOfClustersInSection[i][virtualBottomSectionInterval[i]]) ){return -5;}
                
                

                int numberOfClustersFromSectionsBeforeFirstVisibleSection = 0;
                for(int j =0; j < virtualTopSectionInterval[i]; j++){
                    numberOfClustersFromSectionsBeforeFirstVisibleSection = numberOfClustersFromSectionsBeforeFirstVisibleSection + RepeatingSectionsNumberOfClustersInSection[i][j];
                }

                
                int numberOfClustersFromSectionsInBetweenFirstAndLastVisibleSections = 0;
                for(int j = virtualTopSectionInterval[i]; j < virtualBottomSectionInterval[i]; j++){
                    numberOfClustersFromSectionsInBetweenFirstAndLastVisibleSections = numberOfClustersFromSectionsInBetweenFirstAndLastVisibleSections + RepeatingSectionsNumberOfClustersInSection[i][j];
                }

                FirstVisibleRowLineClusterIndexTop[i] = numberOfClustersFromSectionsBeforeFirstVisibleSection + virtualTopSectionFirstVisibleClusterIndex[i];
                LastVisibleRowLineClusterIndexBottom[i] = numberOfClustersFromSectionsBeforeFirstVisibleSection + numberOfClustersFromSectionsInBetweenFirstAndLastVisibleSections + virtualBottomSectionLastVisibleClusterIndex[i];
            }


            if((previousFirstVisibleRowLineClusterIndexTop[i] != FirstVisibleRowLineClusterIndexTop[i]) || (previousLastVisibleRowLineClusterIndexBottom[i] != LastVisibleRowLineClusterIndexBottom[i])){
                clusterVisibilityHasChanged[i] = 1;
                FirstVisibleRowLineClusterIndexTopDelta[i] = FirstVisibleRowLineClusterIndexTop[i] - previousFirstVisibleRowLineClusterIndexTop[i];//previousFirstVisibleRowLineClusterIndexTop - FirstVisibleRowLineClusterIndexTop;
                LastVisibleRowLineClusterIndexBottomDelta[i] = LastVisibleRowLineClusterIndexBottom[i] - previousLastVisibleRowLineClusterIndexBottom[i];//previousLastVisibleRowLineClusterIndexBottom - LastVisibleRowLineClusterIndexBottom;
            }
            else{
                clusterVisibilityHasChanged[i] = 0;
                FirstVisibleRowLineClusterIndexTopDelta[i] = 0;
                LastVisibleRowLineClusterIndexBottomDelta[i] = 0;
            }
        }

        return 0;


    }

    void updateVisibleClusters(){

        for(int i = 0; i < NumberOfElementPrototypes; i++){


            //First create/delete/shift rowlineclusters/rowlines
            if(FirstVisibleRowLineClusterIndexTopDelta[i] < 0){ //scroll up
                //
                int numberOfRowLinesToBeCleared = 0;
                for(int j = 0; j < (-FirstVisibleRowLineClusterIndexTopDelta[i]); j++){
                    numberOfRowLinesToBeCleared = numberOfRowLinesToBeCleared + RowLineClusters[j].NumberOfRowLinesInCluster;
                }
                

                memmove(RowLineBuffer + numberOfRowLinesToBeCleared,RowLineBuffer, sizeof(rowLineAlignment) * RowLineBufferIndex);
                memmove(RowLineClusters + (-FirstVisibleRowLineClusterIndexTopDelta[i]),RowLineClusters, sizeof(rowLineCluster) * RowLineClusterIndex);
                //for(int j = 0; j < numberOfRowLinesToBeCleared; j++){
                //    RowLineBuffer[j].deleteRowLineAlignment();
                //}
                //for(int j = 0; j < (-FirstVisibleRowLineClusterIndexTopDelta[i]); j++){
                //    RowLineClusters[j].freeMemory();
                //}

                for(int j = (-FirstVisibleRowLineClusterIndexTopDelta[i]); j < (RowLineClusterIndex + (-FirstVisibleRowLineClusterIndexTopDelta[i])); j++){
                    RowLineClusters[j].StartingRowLineIndex = RowLineClusters[j].StartingRowLineIndex + numberOfRowLinesToBeCleared;
                    
                }
                

                memset(RowLineBuffer,0,sizeof(rowLineAlignment) * numberOfRowLinesToBeCleared);
                memset(RowLineClusters,0,sizeof(rowLineCluster) * (-FirstVisibleRowLineClusterIndexTopDelta[i]));


                //note that initializeRowLineCluster will in turn initialize the rowlines it needs
                int newClusterYOffsets = 0;
                int numberOfRowlinesAddedIn = 0;
                for(int j = 0; j < (-FirstVisibleRowLineClusterIndexTopDelta[i]); j++){
                    int repeatingSectionIndex = 0; //todo
                    int maxNumerOfElementsInCluster = RepeatingSectionsNumberOfElementsPerCluster[i][repeatingSectionIndex];
                    int startingRowlineIndex = numberOfRowlinesAddedIn; //todo
                    int numberOfRowlinesInCluster = 0; //todo
                    int elementListStartIndex =0; int elementListEndIndex =0; //todo
                    bool isSpecialCluster = 0;
                    int numberOfInsertionPoints; int* insertionPoints; int* numberOfExtraRowlinesAtInsertionPoint;
                    RowLineClusters[i].initializeRowLineCluster2(newClusterYOffsets,j,MaxNumberOfElementsPerCluster,this,startingRowlineIndex,numberOfRowlinesInCluster,&ElementPrototypes[i],
                                                                 elementListStartIndex,elementListEndIndex,numberOfExtraRowlinesAtInsertionPoint);
                }
            }

            else if(FirstVisibleRowLineClusterIndexTopDelta[i] > 0){ //scroll down
                //clear out the first FirstVisibleRowLineClusterIndexTopDelta clusters and all its rowlines
                int numberOfRowLinesToBeCleared = 0;
                for(int j = 0; j < FirstVisibleRowLineClusterIndexTopDelta[i]; j++){ 
                    numberOfRowLinesToBeCleared = numberOfRowLinesToBeCleared + RowLineClusters[j].NumberOfRowLinesInCluster;
                    RowLineClusters[j].freeMemory();
                }
                memset(RowLineClusters,0,sizeof(rowLineCluster) * FirstVisibleRowLineClusterIndexTopDelta[i]);

                for(int j = 0; j < numberOfRowLinesToBeCleared; j++){
                    RowLineBuffer[j].deleteRowLineAlignment();
                }
                memset(RowLineBuffer,0,sizeof(rowLineAlignment) * numberOfRowLinesToBeCleared);



                // shift the clusters and rowlines into the recently cleared spaces
            
                int rowLineIndex = RowLineClusters[FirstVisibleRowLineClusterIndexTopDelta[i]].StartingRowLineIndex;
                memmove(RowLineBuffer, RowLineBuffer + rowLineIndex,sizeof(rowLineAlignment) * (RowLineBufferIndex - numberOfRowLinesToBeCleared));
                memmove(RowLineClusters,RowLineClusters+FirstVisibleRowLineClusterIndexTopDelta[i],sizeof(rowLineCluster) * (RowLineClusterIndex-FirstVisibleRowLineClusterIndexTopDelta[i]));
            }

                
            
            //
            if(LastVisibleRowLineClusterIndexBottomDelta[i] < 0){ //scroll up
                int Index = RowLineClusterIndex - FirstVisibleRowLineClusterIndexTopDelta[i];
                if(Index < RowLineClusterIndex){ //topDelta scrolled down
                    for(int i = Index; i < RowLineClusterIndex; i++){
                        RowLineClusters[i].freeMemory();

                    }
                    memset(RowLineClusters + Index,0, (-LastVisibleRowLineClusterIndexBottomDelta[i]) * sizeof(rowLineCluster));

                }


                else if(Index > RowLineClusterIndex){ //top delta scrolled up
                    for(int i = RowLineClusterIndex; i < Index; i++){
                        RowLineClusters[i].freeMemory();
                    }
                    memset(RowLineClusters + RowLineClusterIndex,0, (-LastVisibleRowLineClusterIndexBottomDelta[i]) * sizeof(rowLineCluster));
                }
                else{ //adding this here for now because shouldnt there be a case when topDelta is 0? as in the case of a region resize?

                }

            }

            else if(LastVisibleRowLineClusterIndexBottomDelta[i] > 0){ //scroll down
                //zero out
                int startIndex = RowLineClusterIndex - FirstVisibleRowLineClusterIndexTopDelta[i];
                if(startIndex < RowLineClusterIndex){
                    for(int i = startIndex; i < RowLineClusterIndex; i++){
                        RowLineClusters[i].freeMemory();
                    }
                    memset((RowLineClusters+startIndex),0,(RowLineClusterIndex-startIndex) * sizeof(rowLineCluster));

                }

                for(int i =0; i < LastVisibleRowLineClusterIndexBottomDelta[i]; i++){
                    if(startIndex+i > MaxNumberOfRowLineClusters){/*HARD CRASH*/}
                    //RowLineClusters[startIndex+i].initializeRowLineCluster();

                }
                

            }



            RowLineClusterIndex = (LastVisibleRowLineClusterIndexBottom[i] - FirstVisibleRowLineClusterIndexTop[i]) + 1;


    
        }
        //then once that is done use virtualTop/virtualBottom to change/update the rawRowLineAlignmentY values
        
    }

    //this retrieves the visual location [clusterindex,elementindex] from a given ElementListIndex
    //if the returned location contains clusterIndex < 0, then this means that this cluster is NOT VISIBLE
    //This is useful for when deselecting elements in the "ElementList", and we need to check if they need to be visually deselected on screen
    int getElementVisualLocation(int elementType,int elementListIndice,int* returnLocationIndices){
        int sectionIndex = -1;
        int numberOfElementsFromCurrentAndPrecedingSections = 0;
        for(int i = 0; i < NumberOfSections[elementType]; i++){
            numberOfElementsFromCurrentAndPrecedingSections = (RepeatingSectionsNumberOfClustersInSection[elementType][i] * RepeatingSectionsNumberOfElementsPerCluster[elementType][i]);
            if((elementListIndice / numberOfElementsFromCurrentAndPrecedingSections) == 0){
                sectionIndex = i;
                break;


            }
            
            
        }

        if(sectionIndex == -1){return -1;}


        int numberOfElementsFromPrecedingSections =  numberOfElementsFromCurrentAndPrecedingSections - (RepeatingSectionsNumberOfClustersInSection[elementType][sectionIndex] * RepeatingSectionsNumberOfElementsPerCluster[elementType][sectionIndex]);
        
        
        int visualElementIndex = elementListIndice - (numberOfElementsFromPrecedingSections-1);
        int visualClusterIndex = visualElementIndex / RepeatingSectionsNumberOfClustersInSection[elementType][sectionIndex];

        returnLocationIndices[0] = visualClusterIndex;
        returnLocationIndices[1] = visualElementIndex;

        return 0;






    }

    int getElementListIndex(int elementType, int* locationIndices){
        int clusterIndex = locationIndices[0];
        int elementIndex = locationIndices[1];

        int numberOfClustersFromCurrentAndPrecedingSections = 0;
        for(int i = 0; i < NumberOfSections[elementType]; i++){
            numberOfClustersFromCurrentAndPrecedingSections = numberOfClustersFromCurrentAndPrecedingSections + RepeatingSectionsNumberOfClustersInSection[elementType][i];
            if(clusterIndex - numberOfClustersFromCurrentAndPrecedingSections){

            }
        }

        return 0;

    }


    //this should be made private
    void initializeRowLineRegion(int type,int rowLineBufferSize,int maxNumberOfRowLineClusters,int leftSideXValue, int rightSideXValue, int topSideYValue, int bottomSideYValue,int numberOfElementPrototypes,rowLineRegionElementInfo* elementPrototypes,int numberOfElementsPerCluster, int maxNumberOfMouseSelections, int maxNumberOfSystemSelections, int* maxNumberOfElements, int* maxNumberOfSections){
        printf("\n\n");
        printf("DEBUGGING initializeRowLineRegion() : 0 \n");
        Type = type;

        NumberOfElementPrototypes = numberOfElementPrototypes;
        ElementPrototypes = elementPrototypes;

         printf("DEBUGGING initializeRowLineRegion() : 0.5 : NumberOfElementPrototypes = %d  : ElementPrototypes[0].numberOfRowLinesPerElement = %d \n",NumberOfElementPrototypes,ElementPrototypes[0].numberOfRowLinesPerElement);
         for(int i = 0; i < NumberOfElementPrototypes; i++){
            for(int j = 0; j < ElementPrototypes[i].numberOfRowLinesPerElement; j++){
                if(ElementPrototypes[i].ordering[j] == 0){
                    printf("if(ElementPrototypes[i].ordering[j] == 0) \n");
                    printf("ElementPrototypes[i].textBoxes[j].RowLineAlignmentPixelCoordinateY = %d \n",ElementPrototypes[i].textBoxes[j].RowLineAlignmentPixelCoordinateY);
                    printf("ElementPrototypes[i].textBoxes[j].Height = %d \n",ElementPrototypes[i].textBoxes[j].Height);
                    

                }
                if(ElementPrototypes[i].ordering[j] == 1){
                    printf("if(ElementPrototypes[i].ordering[j] == 1) \n");
                    printf("ElementPrototypes[i].VisualElements[j].RowLineAlignmentPixelCoordinateY = %d \n",ElementPrototypes[i].VisualElements[j].RowLineAlignmentPixelCoordinateY);
                    printf("ElementPrototypes[i].VisualElements[j].Height = %d \n",ElementPrototypes[i].VisualElements[j].Height);

                    
                }
            }
         }

        MaxNumberOfRowLineClusters = maxNumberOfRowLineClusters;


        LeftSideXValue = leftSideXValue;
        RightSideXValue = rightSideXValue;
        TopSideYValue = topSideYValue;
        BottomSideYValue = bottomSideYValue;

        VirtualLeftSideXValue   = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);
        VirtualRightSideXValue  = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);
        VirtualTopSideYValue    = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);
        VirtualBottomSideYValue = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);

        for(int i = 0; i < NumberOfElementPrototypes; i++){
            VirtualLeftSideXValue[i] = leftSideXValue;
            VirtualRightSideXValue[i] = rightSideXValue;
            VirtualTopSideYValue[i] = topSideYValue;
            VirtualBottomSideYValue[i] = bottomSideYValue;
        }

        printf("DEBUGGING initializeRowLineRegion() : 1 \n");

       


        

        MaxNumberOfElements = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);
        NumberOfElements = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);

        TextBoxStrings = (char***)malloc(sizeof(char**) * NumberOfElementPrototypes);
        visualElementID = (int**)malloc(sizeof(int*) * NumberOfElementPrototypes);
        elementHighlights = (int**)malloc(sizeof(int*) * NumberOfElementPrototypes);
        elementMouseSelected = (int**)malloc(sizeof(int*) * NumberOfElementPrototypes);

        
        specialElement = (int**)malloc(sizeof(int*) * NumberOfElementPrototypes);
        NumberOfSpecialElements = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);

        specialElement_TextWrapAround_NumberOfTotalExtraRowlines = (int**)malloc(sizeof(int*) * NumberOfElementPrototypes);
        specialElement_TextWrapAround_NumberOfInsertionPoints = (int**)malloc(sizeof(int*) * NumberOfElementPrototypes);
        specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline = (int***)malloc(sizeof(int**) * NumberOfElementPrototypes);
        specialElement_TextWrapAround_InsertionPoints = (int***)malloc(sizeof(int**) * NumberOfElementPrototypes);
        specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint = (int***)malloc(sizeof(int**) * NumberOfElementPrototypes);


        printf("DEBUGGING initializeRowLineRegion() : 2 \n");
        
        for(int i = 0; i < NumberOfElementPrototypes; i++){

            //SETUP THE ELEMENT LIST
            MaxNumberOfElements[i] = maxNumberOfElements[i];
            NumberOfElements[i] = 0;

            TextBoxStrings[i] = (char**)malloc(sizeof(char*) *  ElementPrototypes[i].numberOfTextBoxesInElement * MaxNumberOfElements[i]);
            memset(TextBoxStrings[i],0, sizeof(char*) * ElementPrototypes[i].numberOfTextBoxesInElement * MaxNumberOfElements[i]);
            visualElementID[i] = (int*)malloc(sizeof(int) * ElementPrototypes[i].numberOfVisualElementsInElement * MaxNumberOfElements[i]);
            memset(visualElementID[i],0, sizeof(int) * ElementPrototypes[i].numberOfVisualElementsInElement * MaxNumberOfElements[i]);
            elementHighlights[i] = (int*)malloc(sizeof(int) * MaxNumberOfElements[i]);
            memset(elementHighlights[i], 0, sizeof(int) * MaxNumberOfElements[i]);      //note that this means elements are initialized with a valid value - the "unhighlighted" value
        
            elementMouseSelected[i] = (int*)malloc(sizeof(int) * MaxNumberOfElements[i]);
            memset(elementMouseSelected[i],0,sizeof(int) * MaxNumberOfElements[i]);

            specialElement[i] = (int*)malloc(sizeof(int) * MaxNumberOfElements[i]);
            for(int j = 0; j < MaxNumberOfElements[i]; j++){
                specialElement[i][j] = -1;
            }
            NumberOfSpecialElements[i] = 0;

            specialElement_TextWrapAround_NumberOfTotalExtraRowlines[i] = (int*)malloc(sizeof(int) * MaxNumberOfElements[i]);
            memset(specialElement_TextWrapAround_NumberOfTotalExtraRowlines[i],0,sizeof(int) * MaxNumberOfElements[i]);

            specialElement_TextWrapAround_NumberOfInsertionPoints[i] = (int*)malloc(sizeof(int) * MaxNumberOfElements[i]); 
            memset(specialElement_TextWrapAround_NumberOfInsertionPoints[i],0,sizeof(int) * MaxNumberOfElements[i]);

            specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[i] = (int**)malloc(sizeof(int*) * MaxNumberOfElements[i]);
            specialElement_TextWrapAround_InsertionPoints[i] = (int**)malloc(sizeof(int*) * MaxNumberOfElements[i]);
            specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[i] = (int**)malloc(sizeof(int*) * MaxNumberOfElements[i]);
            for(int j = 0; j < MaxNumberOfElements[i]; j++){
                specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[i][j] = (int*)malloc(sizeof(int) * ElementPrototypes[i].numberOfTextBoxesInElement);
                specialElement_TextWrapAround_InsertionPoints[i][j] = (int*)malloc(sizeof(int) * ElementPrototypes[i].numberOfTextBoxesInElement);
                specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[i][j] = (int*)malloc(sizeof(int) * ElementPrototypes[i].numberOfTextBoxesInElement);
                for(int k =0; k < ElementPrototypes[i].numberOfTextBoxesInElement; k++){
                    specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[i][j][k] = 0;
                    specialElement_TextWrapAround_InsertionPoints[i][j][k] = -1;
                    specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[i][j][k] = 0;
                }
            }

            
        }
        //Note that textBoxStrings[i][j] is not malloc'd during intializeRowLineRegion() and is instead set to 0, and is meant to be allocated "on the fly"
        printf("DEBUGGING initializeRowLineRegion() : 3 \n");

        NumberOfSections = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);
        MaxNumberOfSections = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);
	    RepeatingSectionsNumberOfClustersInSection = (int**)malloc(sizeof(int*) * NumberOfElementPrototypes);
	    RepeatingSectionsNumberOfElementsPerCluster = (int**)malloc(sizeof(int*) * NumberOfElementPrototypes);
        RepeatingSectionsRowLineClusterAndSpacingHeight = (int**)malloc(sizeof(int*) * NumberOfElementPrototypes);
        RepeatingSectionsStartingPixelYCoordinates = (int**)malloc(sizeof(int*) * NumberOfElementPrototypes);
	    RepeatingSectionIsSpecial = (int**)malloc(sizeof(int*) * numberOfElementPrototypes);
	    NumberOfSpecialRepeatingSections = (int*)malloc(sizeof(int) * numberOfElementPrototypes);
	
        specialCluster_TotalNumberOfExtraRowLines = (int**)malloc(sizeof(int*) * numberOfElementPrototypes);
        specialCluster_NumberOfInsertionPoints = (int**)malloc(sizeof(int*) * numberOfElementPrototypes);
        specialCluster_TotalExtraYOffset = (int**)malloc(sizeof(int*) * numberOfElementPrototypes);
        specialCluster_InsertionPointYOffsetForNextRowline = (int***)malloc(sizeof(int**) * numberOfElementPrototypes);
        specialCluster_InsertionPoints = (int***)malloc(sizeof(int**) * numberOfElementPrototypes);
        specialCluster_NumberOfExtraRowlinesNeededAtInsertionPoint = (int***)malloc(sizeof(int**) * numberOfElementPrototypes);
        printf("DEBUGGING initializeRowLineRegion() : 4 \n");
        for(int i = 0; i < NumberOfElementPrototypes; i++){
            NumberOfSections[i] = 0;
            MaxNumberOfSections[i] = maxNumberOfSections[i];
            RepeatingSectionsNumberOfClustersInSection[i] = (int*)malloc(sizeof(int) * MaxNumberOfSections[i]);
            RepeatingSectionsNumberOfElementsPerCluster[i] = (int*)malloc(sizeof(int) * MaxNumberOfSections[i]);
            RepeatingSectionsRowLineClusterAndSpacingHeight[i] = (int*)malloc(sizeof(int) * MaxNumberOfSections[i]);
            RepeatingSectionsStartingPixelYCoordinates[i] = (int*)malloc(sizeof(int) * MaxNumberOfSections[i]);
            RepeatingSectionIsSpecial[i] = (int*)malloc(sizeof(int) * MaxNumberOfSections[i]);
            NumberOfSpecialRepeatingSections[i] = 0;

            specialCluster_TotalNumberOfExtraRowLines[i] = (int*)malloc(sizeof(int) * MaxNumberOfSections[i]);
            specialCluster_NumberOfInsertionPoints[i] = (int*)malloc(sizeof(int) * MaxNumberOfSections[i]);
            specialCluster_TotalExtraYOffset[i] = (int*)malloc(sizeof(int) * MaxNumberOfSections[i]);


            specialCluster_InsertionPointYOffsetForNextRowline[i] = (int**)malloc(sizeof(int*) * MaxNumberOfSections[i]);
            specialCluster_InsertionPoints[i] = (int**)malloc(sizeof(int*) * MaxNumberOfSections[i]);
            specialCluster_NumberOfExtraRowlinesNeededAtInsertionPoint[i] = (int**)malloc(sizeof(int*) * MaxNumberOfSections[i]);

            for(int j = 0; j < MaxNumberOfSections[i]; j++){

                RepeatingSectionsNumberOfClustersInSection[i][j] = 0;
                RepeatingSectionsNumberOfElementsPerCluster[i][j] = 0;
                RepeatingSectionsRowLineClusterAndSpacingHeight[i][j] = 0;
                RepeatingSectionsStartingPixelYCoordinates[i][j] = 0;
                RepeatingSectionIsSpecial[i][j] = 0;

                specialCluster_TotalNumberOfExtraRowLines[i][j] = 0;
                specialCluster_NumberOfInsertionPoints[i][j] = 0;
                specialCluster_TotalExtraYOffset[i][j] = 0;

                
                
                


                specialCluster_InsertionPointYOffsetForNextRowline[i][j] = (int*)malloc(sizeof(int) * ElementPrototypes[i].numberOfTextBoxesInElement);
                specialCluster_InsertionPoints[i][j] = (int*)malloc(sizeof(int) * ElementPrototypes[i].numberOfTextBoxesInElement);
                specialCluster_NumberOfExtraRowlinesNeededAtInsertionPoint[i][j] = (int*)malloc(sizeof(int) * ElementPrototypes[i].numberOfTextBoxesInElement);
                for(int k =0; k < ElementPrototypes[i].numberOfTextBoxesInElement; k++){
                    specialCluster_InsertionPointYOffsetForNextRowline[i][j][k] = 0;
                    specialCluster_InsertionPoints[i][j][k] = -1;
                    specialCluster_NumberOfExtraRowlinesNeededAtInsertionPoint[i][j][k] = 0;
                }
	   
	        }

        }
        printf("DEBUGGING initializeRowLineRegion() : 5 \n");


        MaxNumberOfElementsPerCluster = numberOfElementsPerCluster;


        needsRefresh[0] = 1; // 1 == full refresh, 2 == partial refresh
        needsRefresh[1] = 0; //0 ==no refresh, 1 == new mouse select
        needsRefresh[2] = 0; //0 ==no refresh, 1 == mouseSelection needs update
        needsRefresh[3] = 0; //no highlight curr        
        needsRefresh[4] = 0; //no highlight prev 
        needsRefresh[5] = 0; //

        RowLineBufferSize = rowLineBufferSize;
      
        RowLineBufferIndex = 0;
        RowLineBuffer = (rowLineAlignment*)malloc(sizeof(rowLineAlignment) * RowLineBufferSize);

        //PrototypeRowlineClusters = (rowLineCluster*)malloc(sizeof(rowLineCluster) * numberOfElementPrototypes);
        //for(int i = 0; i < NumberOfElementPrototypes; i++){
        //    PrototypeRowlineClusters[i].initializeRowLineCluster(0,0,0,0,this,0,elementPrototypes[i].numberOfRowLinesPerElement,&elementPrototypes[i],0);
        //}
        

        nextRowLineClusterYOffset = 0;

        printf("DEBUGGING initializeRowLineRegion() : 6 \n");

        //section 0: from a to b-1
        //section 1: from b to c-1
        //section 2: from c to d-1
        //repeatingSectionsStartingPixelYCoordinates[][0] = a , repeatingSectionsStartingPixelYCoordinates[][1] = b, repeatingSectionsStartingPixelYCoordinates[][2] = c


    
      
        //CurrentElementRowClusterIndex = 0;
        


        //printf("initializeRowLineRegion(): RowLineBufferSize = %d \n ",RowLineBufferSize);


        RowLineClusterIndices = (int*)(malloc(sizeof(int) * MaxNumberOfRowLineClusters));
        RowLineClusterIndices[0] = 0;


        RowLineClusters = (rowLineCluster*)malloc(sizeof(rowLineCluster) * MaxNumberOfRowLineClusters);


        

        //get element width
        //ElementWidth = 0;
        //ElementHeight = 0;


            
        printf("DEBUGGING initializeRowLineRegion() : 7 \n");
     


        RowLineClusterIndex = 0;

       
        FirstVisibleRowLineIndexTop = 0;
        LastVisibleRowLineIndexBottom = -1;


        FirstVisibleRowLineClusterIndexTop = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);
        LastVisibleRowLineClusterIndexBottom = (int*)malloc(sizeof(int) * NumberOfElementPrototypes);

        for(int i = 0; i < NumberOfElementPrototypes; i++){
            FirstVisibleRowLineClusterIndexTop[i] = 0;
            LastVisibleRowLineClusterIndexBottom[i] = -1;
        }

        

        NumberOfMouseSelectedElements = 0;
        NumberOfSystemSelectedElements = 0;

        
        RowLineClusterIndices[RowLineClusterIndex] = RowLineBufferIndex;
        


        printf("DEBUGGING initializeRowLineRegion() : 8 \n");

        MaxNumberOfSystemSelectedElements = maxNumberOfSystemSelections;
        MaxNumberOfMouseSelectedElements = maxNumberOfMouseSelections;


        mouseSelectedLocation = (int*)malloc(sizeof(int) * MaxNumberOfMouseSelectedElements);
        for(int i = 0; i < MaxNumberOfMouseSelectedElements; i++){
            mouseSelectedLocation[i] = -1;
        }
        


        systemSelectedLocation = (int*)malloc(sizeof(int) * MaxNumberOfSystemSelectedElements);
        for(int i = 0; i <MaxNumberOfSystemSelectedElements; i++){
            systemSelectedLocation[i] = -1;
        }
        
        printf("DEBUGGING initializeRowLineRegion() : 9 \n");
        printf("DEBUGGING initializeRowLineRegion() : 9.5 : NumberOfElementPrototypes = %d  : ElementPrototypes[0].numberOfRowLinesPerElement = %d \n",NumberOfElementPrototypes,ElementPrototypes[0].numberOfRowLinesPerElement);
    } 

    void resize(int borderID, int offset){
        freeMemory();
        int* sideValues[4] = {&TopSideYValue, &RightSideXValue, &BottomSideYValue, &LeftSideXValue};
        sideValues[borderID][0] = sideValues[borderID][0] + offset;
        initializeRowLineRegion(Type,RowLineBufferSize,MaxNumberOfRowLineClusters,sideValues[0][0],sideValues[1][0],sideValues[2][0],sideValues[3][0],NumberOfElementPrototypes,ElementPrototypes,MaxNumberOfElementsPerCluster,MaxNumberOfMouseSelectedElements,MaxNumberOfSystemSelectedElements,MaxNumberOfElements,MaxNumberOfSections);

        

    }


    
    void createRowLine(bool textBoxOrUiElement,int bufferSize,int pixelCoordinateY,int layer){
        RowLineBuffer[RowLineBufferIndex] = rowLineAlignment(textBoxOrUiElement,bufferSize,layer,pixelCoordinateY);
        RowLineBufferIndex = RowLineBufferIndex + 1;
    }
    

    void createRowLineCluster(int type,int clusterVerticalOffset, rowLineRegionElementInfo* elementPrototype,int horizontalSpacing,int maxNumberOfElements,int textBoxLength = 0,int textBoxMaxWidth = -1){
        printf("createRowLineCluster() :: \n");
        printf("createRowLineCluster() :: elementPrototype->firstElementPixelCoordinateX = %d \n",elementPrototype->firstElementPixelCoordinateX);
        

        RowLineClusters[RowLineClusterIndex].initializeRowLineCluster(type,clusterVerticalOffset,RowLineClusterIndex,maxNumberOfElements,this,RowLineBufferIndex,elementPrototype->numberOfRowLinesPerElement,elementPrototype,horizontalSpacing);
        RowLineClusterIndex = RowLineClusterIndex + 1;
        RowLineBufferIndex = RowLineBufferIndex + elementPrototype->numberOfRowLinesPerElement;


    }
    
    
    void appendElement(int* uiElementIDs, char** textBoxStrings, int elementType = 0){
        //so now this will firstly write to a log listing out all of the added elements that are
        //contained within this rowlineregion, 

        int addElementToClusterReturnVal = RowLineClusters[RowLineClusterIndex-1].addElement2(uiElementIDs,textBoxStrings);
        
        if(addElementToClusterReturnVal == 1){
            nextRowLineClusterYOffset = nextRowLineClusterYOffset + RowLineClusters[RowLineClusterIndex-1].ElementHeight + ElementPrototypes[elementType].verticalSpacing; //+ Spacing;
            int prototypeStringWidth = -1; 
            if(Type == 1 && ElementPrototypes[elementType].ordering[ElementPrototypes[elementType].numberOfRowLinesPerElement-1] == 0){    
                prototypeStringWidth = ElementPrototypes[elementType].textBoxes[ElementPrototypes[elementType].numberOfRowLinesPerElement-1].Width;
            }
            createRowLineCluster(Type,nextRowLineClusterYOffset,ElementPrototypes + elementType,ElementPrototypes[elementType].horizontalSpacing,MaxNumberOfElementsPerCluster,40,prototypeStringWidth); //for now just assume to alsways use first prototype
            RowLineClusterIndices[RowLineClusterIndex] = RowLineBufferIndex;      
        }
        NumberOfElements[elementType] = NumberOfElements[elementType] + 1;
    }

    

    void appendRowLineToCluster(int clusterIndex);



    void findRowLineClusterIndex(){


    }

   

    

    void updateVisibleIndices(){
        //printf("\n\n\n\n\n\n\n\n\n\n\nDEBUGGING updateLastVisibleIndices() : \n");
        for(int i = 0; i < NumberOfElementPrototypes; i++){
            bool firstVisibleRowFound = 0;
            LastVisibleRowLineIndexBottom = 0;
            //printf("DEBUGGING updateLastVisibleIndices() : RowLineBufferIndex = %d \n",RowLineBufferIndex);
            for(int j = 0; j < RowLineBufferIndex; j++){
                //printf("DEBUGGING updateLastVisibleIndices() : i = %d \n",i);
                if(isRowLineVisibleInRowLineRegion(j)){
                    //printf("DEBUGGING updateLastVisibleIndices() : i = %d : hit if \n",i);
                    if(firstVisibleRowFound == 0){
                        FirstVisibleRowLineIndexTop = j;
                        firstVisibleRowFound = 1;
                    }
                    //else?
                    LastVisibleRowLineIndexBottom = j;

                }
            
            }
            

            FirstVisibleRowLineClusterIndexTop[i] = binaryIntervalSearch(RowLineClusterIndices,RowLineClusterIndex+1,FirstVisibleRowLineIndexTop);
            LastVisibleRowLineClusterIndexBottom[i] = binaryIntervalSearch(RowLineClusterIndices,RowLineClusterIndex+1,LastVisibleRowLineIndexBottom);
        

            int distance = RowLineBuffer[RowLineBufferIndex-1].PixelCoordinateY - BottomSideYValue;
            if(distance < 0){
                DistanceToLastRowLine = -2147483648;

            }
            else{
                DistanceToLastRowLine = distance;

            }
        }
       
    }

    bool isRowLineVisibleInRowLineRegion(int rowLineIndex/*,updateBlock* returnOverlappingRegionUpdateBlock = 0*/){
        //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : rowLineIndex = %d : \n",rowLineIndex);
        RowLineBuffer[rowLineIndex].updateRowLineBoundingHeight();

        int rowLineUpperLeftX = 0;
        int rowLineUpperLeftY =  RowLineBuffer[rowLineIndex].PixelCoordinateY - RowLineBuffer[rowLineIndex].RowLineBoundingHeight;
        int rowLineBottomRightX = 1920;
        int rowLineBottomRightY = RowLineBuffer[rowLineIndex].PixelCoordinateY;



        //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : rowLineUpperLeftX = %d \n",rowLineUpperLeftX);
        //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : rowLineUpperLeftY = %d \n",rowLineUpperLeftY);
        //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : rowLineBottomRightX = %d \n",rowLineBottomRightX);
        //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : rowLineBottomRightY = %d \n",rowLineBottomRightY);
        //int inputUpdateBlockBottomRightPixelX = inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width;
        //int inputUpdateBlockBottomRightPixelY = inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height;

        int rowLineRegionUpperLeftX = LeftSideXValue;
        int rowLineRegionUpperLeftY = TopSideYValue;//RightSideXValue;
        int rowLineRegionBottomRightX = RightSideXValue;//;
        int rowLineRegionBottomRightY = BottomSideYValue;

        //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : rowLineRegionUpperLeftX = %d \n",rowLineRegionUpperLeftX);
        //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : rowLineRegionUpperLeftY = %d \n",rowLineRegionUpperLeftY);
        //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : rowLineRegionBottomRightX = %d \n",rowLineRegionBottomRightX);
        //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : rowLineRegionBottomRightY = %d \n",rowLineRegionBottomRightY);

        if(((rowLineRegionUpperLeftX <= rowLineBottomRightX) && (rowLineRegionBottomRightX >= rowLineUpperLeftX)) && ((rowLineRegionUpperLeftY <= rowLineBottomRightY )&&(rowLineRegionBottomRightY >= rowLineUpperLeftY)) ){
            //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : returning TRUE \n");
            return true;
        }
        else{
            //printf("DEBUGGING : isRowLineVisibleInRowLineRegion() : returning False \n");
            return false;
        }

        


    }

    bool isUpdateBlockInRowLineRegion(updateBlock inputUpdateBlock,updateBlock* returnOverlappingRegionUpdateBlock = 0){
        bool isOverlapping = 0;

        int inputUpdateBlockBottomRightPixelX = inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width;
        int inputUpdateBlockBottomRightPixelY = inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height;

        
        int rowLineRegionUpperLeftX = LeftSideXValue;
        int rowLineRegionUpperLeftY = TopSideYValue;//RightSideXValue;
        int rowLineRegionBottomRightX = RightSideXValue ;//;
        int rowLineRegionBottomRightY = BottomSideYValue ;

        if(((rowLineRegionUpperLeftX <= inputUpdateBlockBottomRightPixelX) && (rowLineRegionBottomRightX >= inputUpdateBlock.topLeftPixelCoordinateX)) && ((rowLineRegionUpperLeftY <= inputUpdateBlockBottomRightPixelY )&&(rowLineRegionBottomRightY >= inputUpdateBlock.topLeftPixelCoordinateY)) ){
            //printf("returning TRUE \n");
            isOverlapping = 1;
        }
        else{
            //printf("returning False \n");
            isOverlapping = 0;
            return false;
        }

        if(returnOverlappingRegionUpdateBlock != 0){
            int overlapRegionTopLeftX = returnMax(rowLineRegionUpperLeftX,inputUpdateBlock.topLeftPixelCoordinateX);
            int overlapRegionTopLeftY = returnMax(rowLineRegionUpperLeftY,inputUpdateBlock.topLeftPixelCoordinateY);
            int overlapRegionBottomRightX = returnMin(rowLineRegionBottomRightX,inputUpdateBlockBottomRightPixelX);
            int overlapRegionBottomRightY = returnMin(rowLineRegionBottomRightY,inputUpdateBlockBottomRightPixelY);

            int overlapRegionWidth = overlapRegionBottomRightX-overlapRegionTopLeftX;
            int overlapRegionHeight = overlapRegionBottomRightY - overlapRegionTopLeftY;

            returnOverlappingRegionUpdateBlock->ID = inputUpdateBlock.ID;
            returnOverlappingRegionUpdateBlock->width = overlapRegionWidth;
            returnOverlappingRegionUpdateBlock->height = overlapRegionHeight;
            returnOverlappingRegionUpdateBlock->Layer = inputUpdateBlock.Layer;
            returnOverlappingRegionUpdateBlock->pixelIndex = inputUpdateBlock.pixelIndex;
            returnOverlappingRegionUpdateBlock->topLeftPixelCoordinateX = overlapRegionTopLeftX;
            returnOverlappingRegionUpdateBlock->topLeftPixelCoordinateY = overlapRegionTopLeftY;
        }

        return isOverlapping;



    }

    void render(uiData* ui);
    void renderSpecificElement(uiData* ui,int rowLineClusterIndex, int elementIndex);
    
    

    int findMouseLocation(mouse* uiMouse){
        //printf("\t\t rowLineRegion::findMouseLocation() : \n");
        int mouseX = uiMouse->mouseXY.mousePixelCoordinateX;
        int mouseY = uiMouse->mouseXY.mousePixelCoordinateY;
        uiMouse->locationData[0][1] = -1;
        uiMouse->locationData[0][2] = -1;
        uiMouse->locationData[0][3] = -1;

        //check if mouse is in row line region
        if(isPointInRectangle(mouseX, mouseY, LeftSideXValue, TopSideYValue, RightSideXValue, BottomSideYValue)){
            //printf("\t\t rowLineRegion::findMouseLocation() : isPointInRectangle(mouseX, mouseY, LeftSideXValue, TopSideYValue, RightSideXValue, BottomSideYValue) = TRUE \n");
            uiMouse->locationData[0][1] = 0;
            uiMouse->locationDepth[0] = 1;
            //check what visible row line bounding box the mouse is in
            

            //printf("DEBUGGING findUiMouseLocation() : UiPanes[i].RowLineRegion->FirstVisibleRowLineClusterIndexTop = %d \n",FirstVisibleRowLineClusterIndexTop);
            //printf("DEBUGGING findUiMouseLocation() : UiPanes[i].RowLineRegion->LastVisibleRowLineClusterIndexBottom = %d \n",LastVisibleRowLineClusterIndexBottom);
            //printf("DEBUGGING findUiMouseLocation() : UiPanes[i].RowLineRegion->FirstVisibleRowLineIndexTop = %d \n",FirstVisibleRowLineIndexTop);
            //printf("DEBUGGING findUiMouseLocation() : UiPanes[i].RowLineRegion->LastVisibleRowLineIndexBottom = %d \n",LastVisibleRowLineIndexBottom);
            for(int j = FirstVisibleRowLineClusterIndexTop[0]; j <= LastVisibleRowLineClusterIndexBottom[0]; j++){
                //printf("\t\t rowLineRegion::findMouseLocation() : j = %d \n",j);
                

                if(RowLineClusters[j].findMouseLocation(uiMouse)){
                    //printf("\t\t rowLineRegion::findMouseLocation() : j = %d : RowLineClusters[j].findMouseLocation(uiMouse) == TRUE  \n",j);
                    return 2;
                    break;
                }
                

            }
            return 1;

             

            //check if the mouse was in row line region, but not in the bounds of a row line cluster (this usually only happens if spacing is very lage between row line clusters)
            

        }
       
        
        
        else{
            return 0;

        }

        


    }

    bool hasMouseSelection(){
        printf("hasMouseSelection() : (NumberOfMouseSelectedElements > 0) = %d \n", (NumberOfMouseSelectedElements > 0));
        return (NumberOfMouseSelectedElements > 0);

    }

    void modifyMouseSelections(int flag){
        printf("void modifyMouseSelections() : flag = %d \n",flag);
        printf("void modifyMouseSelections() : NumberOfMouseSelectedElements = %d \n",NumberOfMouseSelectedElements);
        if(flag < 0){
            printf("if(flag<0){} : NumberOfMouseSelectedElements = %d \n",NumberOfMouseSelectedElements);
            //NumberOfMouseSelectedElements = -NumberOfMouseSelectedElements; //flag<0 is only called once NumberOfMouseSelectedElements is toggled negative, this toggles it back positive before setting to 0
            if(flag == -2){
                printf("if(flag<0){} : if(flag==-2) : \n");
                //mouseSelectedLocation[0] = mouseSelectedLocation[NumberOfMouseSelectedElements-1];
                //mouseSelectedLocation[1] = mouseSelectedLocation[NumberOfMouseSelectedElements]; 
                mouseSelectedLocation[0] = mouseSelectedLocation[(NumberOfMouseSelectedElements * 2)];
                mouseSelectedLocation[1] = mouseSelectedLocation[(NumberOfMouseSelectedElements * 2) + 1];

                printf("if(flag<0){} : if(flag==-2) : mouseSelectedLocation[0] = %d, mouseSelectedLocation[1] = %d \n",mouseSelectedLocation[0],mouseSelectedLocation[1]);
                memset(mouseSelectedLocation + 2,-9999,sizeof(int) * ((NumberOfMouseSelectedElements-1) * 2));
                NumberOfMouseSelectedElements = 1;
                return;

            }
            memset(mouseSelectedLocation,-9999,sizeof(int) * NumberOfMouseSelectedElements * 2);
            NumberOfMouseSelectedElements = 0;
            return;

        }
        printf("void modifyMouseSelections() : \n");
        int highlightValue = 9999; //this is the code used to enforce that the elements are deselected / their highlights are set to 0, whereas a highlight value of 0 is blocked due to the element being selected
        if(flag == 1){
            printf("void modifyMouseSelections() : flag == 1 \n");
            highlightValue = 3;
        }
        if(flag == 2){
            printf("void modifyMouseSelections() : flag == 2 \n");
            highlightValue = 9998; //force selection
        }

        printf("void modifyMouseSelections() : NumberOfMouseSelectedElements = %d \n",NumberOfMouseSelectedElements);
        for(int i = 0; i < NumberOfMouseSelectedElements * 2; i = i+2){
            printf("void modifyMouseSelections() : for i = %d \n",i);
            //RowLineClusters[mouseSelectedLocation[i]].highlightElement(mouseSelectedLocation[i+1],highlightValue);
            
            //IF mouseSelectedLocation[i] is a VISIBLE rowlinecluster THEN {
            highlightElement(mouseSelectedLocation[i],mouseSelectedLocation[i+1],highlightValue);
            //}

            //UPDATE "elementList"
        }

        if(flag == 0){
            printf("void modifyMouseSelections() : flag == 0 : NumberOfMouseSelectedElements = %d \n", NumberOfMouseSelectedElements);
            NumberOfMouseSelectedElements = -NumberOfMouseSelectedElements;
            printf("void modifyMouseSelections() : flag == 0 : NumberOfMouseSelectedElements = %d \n", NumberOfMouseSelectedElements);
        }

       

        needsRefresh[0] = 2;
        needsRefresh[2] = 1; //signify that all elements selected by mouse require update
        printf("void modifyMouseSelections() : needsRefresh[0] = %d, needsRefresh[2] = %d \n",needsRefresh[0],needsRefresh[2]);

    }

    int highlightElement(int rowLineClusterIndex,int elementIndex, int highlightValue){
        
        return RowLineClusters[rowLineClusterIndex].highlightElement(elementIndex,highlightValue);
        

    }

    //elementIndex is the index of the element in reference to the whole rowlineregion
    int selectElement(int rowLineClusterIndex, int elementIndex){
        printf("selectElement : \n");
        //mouseSelectedLocation[(NumberOfMouseSelectedElements*2)] = rowLineClusterIndex;
        //mouseSelectedLocation[(NumberOfMouseSelectedElements*2)+1] = elementIndex;
        //NumberOfMouseSelectedElements = NumberOfMouseSelectedElements + 1;

        if(highlightElement(rowLineClusterIndex,elementIndex,2) == 0){
            printf("if(){} : NumberOfMouseSelectedElements = %d \n",NumberOfMouseSelectedElements);
            //mouseSelectedLocation[(NumberOfMouseSelectedElements*2)] = rowLineClusterIndex;
            //mouseSelectedLocation[(NumberOfMouseSelectedElements*2)+1] = elementIndex;
            if(NumberOfMouseSelectedElements >=0 ){
                mouseSelectedLocation[(NumberOfMouseSelectedElements*2)] = rowLineClusterIndex;
                mouseSelectedLocation[(NumberOfMouseSelectedElements*2)+1] = elementIndex;
                NumberOfMouseSelectedElements = NumberOfMouseSelectedElements + 1;
            }
            else{
                NumberOfMouseSelectedElements = -NumberOfMouseSelectedElements;
                mouseSelectedLocation[(NumberOfMouseSelectedElements*2)] = rowLineClusterIndex;
                mouseSelectedLocation[(NumberOfMouseSelectedElements*2)+1] = elementIndex;
                NumberOfMouseSelectedElements = -NumberOfMouseSelectedElements;

             

                


            }
            needsRefresh[0] = 2;
            needsRefresh[1] = 1;
            return 0;

        }
        return -1;

        
        
        

    }

    int setMultiSelect(int offOrOn){
        isMouseMultiSelectActive = offOrOn;


    }
    



    void scrollRowLineRegion(){
        /*
        if(HorizontalScrollBar){

        }
        if(VerticalScrollBar){
            for(int i = 0; i < RowLineBufferIndex; i++){
                RowLineBuffer[i].PixelCoordinateY - (VerticalScrollBar->ScrollPercent * DistanceToLastRowLine);
            }

        }
        */


    }


    void freeMemory(){
        for(int i = 0; i < RowLineBufferIndex; i++){
            RowLineBuffer[i].deleteRowLineAlignment();    
        }
        free(RowLineBuffer);


        for(int i = 0; i < RowLineClusterIndex; i++){
            RowLineClusters[i].freeMemory();
        }

        free(RowLineClusterIndices);


    }


    void DebugElementList(int elementType){
        printf("\n\n\n DEBUG DebugElementList: \n ");
        rowLineRegionElementInfo prototypeElement = ElementPrototypes[elementType];
        printf("NumberOfElementPrototypes = %d \n",NumberOfElementPrototypes);
        printf("MaxNumberOfElements[elementType] = %d \n",MaxNumberOfElements[elementType]);
        printf("NumberOfElements[elementType] = %d \n",NumberOfElements[elementType]);
        printf("\n \n for(int i = 0; i < NumberOfElements[elementType]; i++){ \n");
        for(int i = 0; i < NumberOfElements[elementType]; i++){
        printf("\t i = %d: \n", i);
        printf("elementHighlights[elementType][%d] = %d \n",i,elementHighlights[elementType][i]);
        printf("elementMouseSelected[elementType][%d] = %d \n",i,elementMouseSelected[elementType][i]);
        
        printf("\t\t for(int j =0; j < prototypeElement.numberOfTextBoxesInElement; j++){ \n");
        for(int j = 0; j < prototypeElement.numberOfTextBoxesInElement;j++){
            printf("\t\t\t j = %d: \n",j);
            printf("\t\t\t TextBoxStrings[elementType][%d] = %s \n",(i * prototypeElement.numberOfTextBoxesInElement) + j, TextBoxStrings[elementType][(i * prototypeElement.numberOfTextBoxesInElement) + j]); 
        }

        printf("\t\t for(int k =0; k < prototypeElement.numberOfVisualElementsInElement; k++){ \n");
        for(int k =0; k < prototypeElement.numberOfVisualElementsInElement; k++){
            printf("visualElementID[elementType][%d] = %d \n",(i * prototypeElement.numberOfVisualElementsInElement) + k, visualElementID[elementType][(i * prototypeElement.numberOfVisualElementsInElement) + k]);
        }
        
        
        printf("specialElement[elementType][%d] = %d \n",i,specialElement[elementType][i]);
        }

        printf("NumberOfSpecialElements[elementType] = %d \n ",NumberOfSpecialElements[elementType]);
        printf("\n\n for(int i2=0; i2 < NumberOfSpecialElements[elementType]; i2++) \n");
        for(int i2=0; i2 < NumberOfSpecialElements[elementType]; i2++){
            printf("\t i2 = %d: \n", i2);
            printf("\t specialElement_TextWrapAround_NumberOfTotalExtraRowlines[elementType][i2] = %d \n",specialElement_TextWrapAround_NumberOfTotalExtraRowlines[elementType][i2]);
            printf("\t for(int j2 = 0; j2 < specialElement_TextWrapAround_NumberOfInsertionPoints; j++) \n");
            for(int j2 = 0; j2 < specialElement_TextWrapAround_NumberOfInsertionPoints[elementType][i2]; j2++){
                printf("\t\t j2 = %d: \n", j2);
                printf("\t\t specialElement_TextWrapAround_InsertionPoints[elementType][i2][j2] = %d \n",specialElement_TextWrapAround_InsertionPoints[elementType][i2][j2]);
                printf("\t\t specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[elementType][i2][j2] = %d \n",specialElement_TextWrapAround_InsertionPointYOffsetForNextRowline[elementType][i2][j2]);
                printf("\t\t specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][i2][j2] = %d \n", specialElement_TextWrapAround_NumberOfExtraRowlinesNeededAtInsertionPoint[elementType][i2][j2]);
            }
        }
    }

    void DebugElementList2(int elementType, int elementIndex){

    }






};





class freeBoundingBox{
    public:
    int TopLeftX = 0;
    int TopLeftY = 0;
    int BottomRightX = 0;
    int BottomRightY = 0;

    int NumberOfFreeRowLinesToTrigger;
    int* NumberOfFreeRowLineElementsToTrigger;
    int* TriggerFreeRowLineIndices; //This is an array of n integers, where n is the number of freerowlines to trigger
    int** TriggerFreeElementIndices; //This is an array of n arrays of x integers, the 0th array denotes which elements from triggerFreeRowLineIndices[0] need triggered

    freeBoundingBox(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY,int numberOfFreeRowLinesToTrigger,  int* triggerFreeRowLineIndices, int* numberOfFreeRowLineElementsToTrigger, int** triggerFreeElementIndices){
        TopLeftX = topLeftX;
        TopLeftY = topLeftY;
        BottomRightX = bottomRightX;
        BottomRightY = bottomRightY;
        NumberOfFreeRowLinesToTrigger = numberOfFreeRowLinesToTrigger;
        NumberOfFreeRowLineElementsToTrigger = numberOfFreeRowLineElementsToTrigger;
        TriggerFreeRowLineIndices = triggerFreeRowLineIndices;
        TriggerFreeElementIndices = triggerFreeElementIndices;





    }

    int findMouseLocation(mouse* uiMouse){
        
        if(isPointInRectangle(uiMouse->mouseXY.mousePixelCoordinateX,  uiMouse->mouseXY.mousePixelCoordinateY, TopLeftX, TopLeftY, BottomRightX, BottomRightY )){
            return 1;
            
        }
        return 0;

    }
    void freeMemory(){
        for(int i = 0; i < NumberOfFreeRowLinesToTrigger; i++){
            free(TriggerFreeElementIndices[i]);
        }
        free(TriggerFreeElementIndices);

        free(TriggerFreeRowLineIndices);

        
    }
};

class uiPane{
    private:
    

    public:
    int PaneID;
    int Layer;
    int NeedsRefresh[3]; //NeedsRefresh[0]: This pertains to the pane's backing graphic, 1 indicates partial redraw, 2 indicates full redraw
                         //NeedsRefresh[1]: This pertains to the rowlineregion, 1 indicates the rowlineregion needs refreshed (in some way)
                         //NeedsRefresh[2]: This pertains to the scrollbar, 1 indicates the scrollbar needs refreshed
                         //NeedsRefresh[3]: This pertains to the freeRowLines

    int numberOfRenderRegions;
    int* renderRegions;
    //bool isUpdated = false;

    int BorderSize;

    //-1 indicates NOT resizeable on the axis,
    //0 indicates the ParentPane handles the axis resizing,
    //1 indicates the pane IS resizeable in this axis
    char IsResizeableHorizontalLeft;
    char IsResizeableHorizontalRight;
    char IsResizeableVerticalTop;
    char IsResizeableVerticalBottom;
    


    int LeftSideXValue;
    int RightSideXValue;
    int TopSideYValue;
    int BottomSideYValue;


    scrollBar* Scrollbars; //Scrollbars[0] = vertical, Scrollbars[1] = horizontal

    rowLineRegion* RowLineRegion;

    int numberOfFreeRowLines;
    int FreeRowLinesBufferSize;
    int freeRowLinesNeedRefresh;
    rowLineAlignment* freeRowLines;


    int FreeBoundingBoxBufferSize;
    int NumberOfFreeBoundingBoxes;
    freeBoundingBox* FreeBoundingBoxes;

 
    
    


    int NumberOfPaneConnections;
    int* PaneConnectionIndices;
    bool* PaneBorderConnections;
    //bool* PaneDeleteConnection;


    //int NumberOfSubPanes; // deprecate this
    //uiPane* SubPanes;


    void renderScrollBars(uiData* ui, int refreshPaneID);
    uiPane(bool sentinelPane){
        if(sentinelPane){
            PaneID = 2147483647;
            Layer = 2147483647;
        }

    }

    uiPane(int id,int layer,int resizeableHorizontalL,int resizeableHorizontalR,int resizeableVerticalT,int resizeableVerticalB, int leftSide, int rightSide, int topSide, int bottomSide,int freeBoundingBoxBufferSize){
        
        
        PaneID = id;

        BorderSize = 5;

        IsResizeableHorizontalLeft = resizeableHorizontalL;
        IsResizeableHorizontalRight = resizeableHorizontalR;
        IsResizeableVerticalTop = resizeableVerticalT;
        IsResizeableVerticalBottom = resizeableVerticalB;



        LeftSideXValue = leftSide;
        RightSideXValue = rightSide;
        TopSideYValue  = topSide;
        BottomSideYValue = bottomSide;

       

        NeedsRefresh[0] = 2;
        NeedsRefresh[1] = 0;
        NeedsRefresh[2] = 0;
        

        Layer = layer;

        
        //NumberOfSubPanes = numOfSubPanes;

        FreeBoundingBoxes = 0; //nullptr
        FreeBoundingBoxBufferSize = freeBoundingBoxBufferSize;
        if(FreeBoundingBoxBufferSize > 0){
            FreeBoundingBoxes = (freeBoundingBox*)malloc(sizeof(freeBoundingBox) * FreeBoundingBoxBufferSize);
            NumberOfFreeBoundingBoxes = 0;
        }
       

        RowLineRegion = 0;
        //SubPanes = subPanes;
       
       
    }

    void createRowLineRegion(int type,int rowLineBufferSize,int maxNumberOfRowLineClusters,int maxNumberOfElementsPerCluster,int leftSideXValue, int rightSideXValue, int topSideYValue, int bottomSideYValue,int numberOfElementPrototypes, rowLineRegionElementInfo* elementPrototypes, int maxNumberOfMouseSelections,int maxNumberOfSystemSelections, int* maxNumberOfElements, int* maxNumberOfSections){
        RowLineRegion = (rowLineRegion*)(malloc(sizeof(rowLineRegion)));
        printf("DEBUG uiPane: createRowLineRegion: \n");
        RowLineRegion->initializeRowLineRegion(type,rowLineBufferSize,maxNumberOfRowLineClusters,leftSideXValue,rightSideXValue,topSideYValue,bottomSideYValue,numberOfElementPrototypes,elementPrototypes,maxNumberOfElementsPerCluster,maxNumberOfMouseSelections,maxNumberOfSystemSelections,maxNumberOfElements,maxNumberOfSections);
        NeedsRefresh[1] = 1;

    }

    void createScrollBar(int type, int topLeftPixelX, int topLeftPixelY, int bottomRightPixelX, int bottomRightPixelY,int scrollBarSliderSize,int layer){
        if(Scrollbars == nullptr){
            Scrollbars = (scrollBar*)malloc(sizeof(scrollBar) * 2);
        }
        Scrollbars[type].initializeScrollbar(type,topLeftPixelX,topLeftPixelY,bottomRightPixelX,bottomRightPixelY,scrollBarSliderSize,layer);
    }

    //buffer size will dictate
    void createFreeRowLineBuffer(int bufferSize){
        freeRowLines = (rowLineAlignment*)(malloc(sizeof(rowLineAlignment) * bufferSize));
        FreeRowLinesBufferSize = bufferSize;
        numberOfFreeRowLines = 0;

    }

    //the return value is the index of the created row line alignment
    int createFreeRowLine(bool textOrUiElement,int bufferSize,int rowLineY,int layer){
        if(freeRowLines == nullptr){
            return -1;
        }

        freeRowLines[numberOfFreeRowLines] = rowLineAlignment(textOrUiElement,bufferSize,layer,rowLineY);
        freeRowLines[numberOfFreeRowLines].needsRefresh = 1;
        freeRowLinesNeedRefresh = 1;
        int returnVal = numberOfFreeRowLines;
        numberOfFreeRowLines = numberOfFreeRowLines + 1;
        return returnVal;


    }

    void createFreeBoundingBox(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY,int numberOfFreeRowLinesToTrigger,  int* triggerFreeRowLineIndices,int* triggerNumberOfElements, int** triggerFreeElementIndices){
        FreeBoundingBoxes[NumberOfFreeBoundingBoxes] = freeBoundingBox(topLeftX, topLeftY, bottomRightX, bottomRightY, numberOfFreeRowLinesToTrigger,triggerFreeRowLineIndices,triggerNumberOfElements,triggerFreeElementIndices);
        NumberOfFreeBoundingBoxes = NumberOfFreeBoundingBoxes + 1;
    }


    /*
    //borderID = 0 = top,   1 = right, 2 = Bottom, 3 = Left
    void resizePane(int borderID, int offset, uiData* parentUiData = nullptr){
        if(RowLineRegion){
            RowLineRegion->resize(borderID,offset);

        }
        if(borderID == 0){
            TopSideYValue = TopSideYValue + offset;

        }
        if(borderID == 1){
            RightSideXValue = RightSideXValue + offset;
            
        }
        if(borderID == 2){
            BottomSideYValue = BottomSideYValue + offset;
            
        }
        if(borderID == 3){
            LeftSideXValue = LeftSideXValue + offset;
            
        }
        

        int connectionBorder[4] = {2,3,0,1};
        for(int i =0; i < NumberOfPaneConnections; i++){
            parentUiData->UiPanes[PaneConnectionIndices[i]].resizePane(connectionBorder[borderID],-offset);
        }


    }
    */

    //flag 0 is just to find if mouse is in pane, flag 2 is to see if mouse is in freeBoundingBox within pane
    int findMouseLocation(mouse* uiMouse,int flag=0){
        //printf("\t UIPANE::findMouseLocation() : \n");
        if(flag == 0){
            //printf("\t UIPANE::findMouseLocation() : flag == 0 TRUE : \n");
            //printf("\t UIPANE::findMouseLocation() : uiMouse->mouseXY.mousePixelCoordinateX = %d \n",uiMouse->mouseXY.mousePixelCoordinateX);
            //printf("\t UIPANE::findMouseLocation() : uiMouse->mouseXY.mousePixelCoordinateY = %d \n",uiMouse->mouseXY.mousePixelCoordinateY);
            //printf("\t UIPANE::findMouseLocation() : LeftSideXValue = %d, TopSideYValue = %d, RightSideXValue= %d, BottomSideYValue = %d :  \n",LeftSideXValue,TopSideYValue,RightSideXValue,BottomSideYValue);
            if(isPointInRectangle(uiMouse->mouseXY.mousePixelCoordinateX,  uiMouse->mouseXY.mousePixelCoordinateY, LeftSideXValue, TopSideYValue, RightSideXValue, BottomSideYValue )){
                //printf("\t UIPANE::findMouseLocation() : flag0 is returning 1 \n");
                return 1;
            }
        }

        if(flag == 1){
            uiMouse->locationData[0][1] = -1;
            uiMouse->locationData[0][2] = -1;
            for(int i = 0; i < NumberOfFreeBoundingBoxes; i++){
                if(FreeBoundingBoxes[i].findMouseLocation(uiMouse)){
                    uiMouse->locationData[0][1] = 3;
                    uiMouse->locationData[0][2] = i;
                    return 2;
                }
            }

        }
        
        if(flag == 2){
            //printf("\t UIPANE::findMouseLocation() : FLAG==2 : \n");
            uiMouse->locationData[0][1] = -1;
            uiMouse->locationData[0][2] = -1;
            uiMouse->locationData[0][3] = -1;
            //printf("\t UIPANE::findMouseLocation() : NumberOfFreeBoundingBoxes = %d \n",NumberOfFreeBoundingBoxes);
            for(int i = 0; i < NumberOfFreeBoundingBoxes; i++){
                if(FreeBoundingBoxes[i].findMouseLocation(uiMouse)){
                    //printf("\t UIPANE::findMouseLocation() : FreeBoundingBoxes[i].findMouseLocation(uiMouse) == TRUE, i = %d \n",i);
                    uiMouse->locationData[0][1] = 3;
                    uiMouse->locationData[0][2] = i;
                    return 2;
                }
            }
            //printf("\t TESTING THIS NOW 2 \n");
            
            if(RowLineRegion){
                if(RowLineRegion->findMouseLocation(uiMouse)){
                    //printf("\t UIPANE::findMouseLocation() : RowLineRegion->findMouseLocation(uiMouse) == TRUE \n");
                    return 3;

                }
                
            }
            //printf("\t TESTING THIS NOW 3 \n");
            if(Scrollbars){
                if(Scrollbars[0].Type == 0){
                    if(Scrollbars[0].findMouseLocation(uiMouse)){
                        //printf("\t UIPANE::findMouseLocation() : Scrollbars[0].findMouseLocation(uiMouse) == TRUE \n");
                        return 2;

                    }

                }
                if(Scrollbars[1].Type == 1){
                    if(Scrollbars[1].findMouseLocation(uiMouse)){
                        //printf("\t UIPANE::findMouseLocation() : Scrollbars[1].findMouseLocation(uiMouse) == TRUE \n");
                        return 2;

                    }
                }
            }
            //printf("\t TESTING THIS NOW 4 \n");
            return -1;

        }

        //check if on border
        if(flag == 3){
            
              
            if(isPointInRectangle(uiMouse->mouseXY.mousePixelCoordinateX,  uiMouse->mouseXY.mousePixelCoordinateY,LeftSideXValue,TopSideYValue,LeftSideXValue+BorderSize,BottomSideYValue)){uiMouse->locationData[0][1] = -2; return 1;} //left
            else if(isPointInRectangle(uiMouse->mouseXY.mousePixelCoordinateX,  uiMouse->mouseXY.mousePixelCoordinateY,RightSideXValue - BorderSize,TopSideYValue,RightSideXValue,BottomSideYValue)){uiMouse->locationData[0][1] = -3; return 1;} //right
            else if(isPointInRectangle(uiMouse->mouseXY.mousePixelCoordinateX,  uiMouse->mouseXY.mousePixelCoordinateY,LeftSideXValue,TopSideYValue,RightSideXValue,TopSideYValue+BorderSize)){uiMouse->locationData[0][1] = -4; return 1;} //top
            else if(isPointInRectangle(uiMouse->mouseXY.mousePixelCoordinateX,  uiMouse->mouseXY.mousePixelCoordinateY,LeftSideXValue,BottomSideYValue - BorderSize,RightSideXValue,BottomSideYValue)){uiMouse->locationData[0][1] = -5; return 1;} //bottom
            

            
            


        }

        return 0;

    }

    int addRedrawRegion(int p1x, int p1y, int p2x, int p2y){
        addRedrawRegionAUX(renderRegions,&numberOfRenderRegions,p1x,p1y,p2x,p2y);

    }

/*
    void deletePane(){
        free(RowLineNeedsRefresh);
        for(int i = 0; i < RowLineBufferIndex; i++){
            RowLineBuffer[i].deleteRowLineAlignment();
        }
        free(RowLineBuffer);

    }
*/



};





class uiData{

    public:

    bool isAnimated = 0;

    int renderTargetWidth = 1920;
    int renderTargetHeight = 1080;

    //updateBlock PreviousCursorUpdateBlock = {};

    //int mouseX = renderTargetWidth/2;
    //int mouseY = renderTargetHeight/2;

    mouse uiMouse = {};

    keyBoard uiKeyboard = {};
    mainThread_ControllerState uiMainController = {};

    int KeyPressRecordBufferSize;
    int KeyPressRecordNumberOfEntriesInPass;
    KeyPressRecord* KeyPressRecordStores;
    KeyPressRecord* KeyPressRecordList;
    
    

    int UiPanesBufferSize;
    int UpdateBlockQueueBufferSize;

    int gameViewTopLeftPixelX = 0 ;
    int gameViewTopLeftPixelY = 56;
    int gameViewBottomRightPixelX = 1640;
    int gameViewBottomRightPixelY= 814;
    
    

    int* UpdateBlockQueueIndex;
    updateBlock* UpdateBlockQueue;

    int NumberOfDispatches;
    int* TotalNumberOfPixelsInDispatch;
    int* DispatchStartIndices;
    int* DispatchLastIndices;

    int NumberOfPanes;
    uiPane* UiPanes;

    bool firstProcessUpdate;

    bool isUpdateBlockInPane(updateBlock inputUpdateBlock,int paneIndex, updateBlock* returnOverlappingRegionUpdateBlock = 0){

       int paneUpperLeftX = UiPanes[paneIndex].LeftSideXValue;
       int paneUpperLeftY = UiPanes[paneIndex].TopSideYValue;
       int paneBottomRightX = UiPanes[paneIndex].RightSideXValue;
       int paneBottomRightY = UiPanes[paneIndex].BottomSideYValue;
       return isRectangleInRectangle(inputUpdateBlock.topLeftPixelCoordinateX, inputUpdateBlock.topLeftPixelCoordinateY, inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width, inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height,
                                              paneUpperLeftX,paneUpperLeftY,paneBottomRightX,paneBottomRightY,
                                              returnOverlappingRegionUpdateBlock);

    }

    bool isUpdateBlockInRowLine(updateBlock inputUpdateBlock,int paneIndex,int rowLineIndex){
        //printf("DEBUGGING isUpdateBlockInRowLine \n");
        //printf("DEBUGGING isUpdateBlockInRowLine : UiPanes[paneIndex].RowLineBuffer[rowLineIndex].TextBoxBuffer[0].Height = %d \n", UiPanes[paneIndex].RowLineBuffer[rowLineIndex].TextBoxBuffer[0].Height);
    
        //UiPanes[paneIndex].RowLineBuffer[rowLineIndex].updateRowLineBoundingHeight();
        UiPanes[paneIndex].RowLineRegion->RowLineBuffer[rowLineIndex].updateRowLineBoundingHeight();

        int inputUpdateBlockBottomRightPixelX = inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width;
        int inputUpdateBlockBottomRightPixelY = inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height;
        //printf("DEBUGGING isUpdateBlockInRowLine : inputUpdateBlockBottomRightPixelX : %d \n",inputUpdateBlockBottomRightPixelX);
        //printf("DEBUGGING isUpdateBlockInRowLine : inputUpdateBlockBottomRightPixelY : %d \n",inputUpdateBlockBottomRightPixelY);


        int rowLineUpperLeftX = 0;
        int rowLineUpperLeftY = UiPanes[paneIndex].RowLineRegion->RowLineBuffer[rowLineIndex].PixelCoordinateY - UiPanes[paneIndex].RowLineRegion->RowLineBuffer[rowLineIndex].RowLineBoundingHeight;
        

        int rowLineBottomRightX = renderTargetWidth;
        int rowLineBottomRightY = UiPanes[paneIndex].RowLineRegion->RowLineBuffer[rowLineIndex].PixelCoordinateY;
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineUpperLeftX : %d \n",rowLineUpperLeftX);
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineUpperLeftY : %d \n",rowLineUpperLeftY);
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineBottomRightX : %d \n",rowLineBottomRightX);
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineBottomRightY : %d \n",rowLineBottomRightY);
        
        /*
        if(((rowLineUpperLeftX <= inputUpdateBlockBottomRightPixelX) && (rowLineBottomRightX >= inputUpdateBlock.topLeftPixelCoordinateX)) && ((rowLineUpperLeftY <= inputUpdateBlockBottomRightPixelY )&&(rowLineBottomRightY >= inputUpdateBlock.topLeftPixelCoordinateY)) ){
            return true;
        }
        else{
            return false;
        }
        */

        return isRectangleInRectangle(inputUpdateBlock.topLeftPixelCoordinateX, inputUpdateBlock.topLeftPixelCoordinateY, inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width, inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height,
                               rowLineUpperLeftX,rowLineUpperLeftY,rowLineBottomRightX,rowLineBottomRightY);

        



    }


        bool isUpdateBlockInFreeRowLine(updateBlock inputUpdateBlock,int paneIndex, int rowLineIndex){
        //printf("DEBUGGING isUpdateBlockInRowLine \n");
        //printf("DEBUGGING isUpdateBlockInRowLine : UiPanes[paneIndex].RowLineBuffer[rowLineIndex].TextBoxBuffer[0].Height = %d \n", UiPanes[paneIndex].RowLineBuffer[rowLineIndex].TextBoxBuffer[0].Height);
    
        //UiPanes[paneIndex].RowLineBuffer[rowLineIndex].updateRowLineBoundingHeight();
        UiPanes[paneIndex].freeRowLines[rowLineIndex].updateRowLineBoundingHeight();

        int inputUpdateBlockBottomRightPixelX = inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width;
        int inputUpdateBlockBottomRightPixelY = inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height;
        //printf("DEBUGGING isUpdateBlockInRowLine : inputUpdateBlockBottomRightPixelX : %d \n",inputUpdateBlockBottomRightPixelX);
        //printf("DEBUGGING isUpdateBlockInRowLine : inputUpdateBlockBottomRightPixelY : %d \n",inputUpdateBlockBottomRightPixelY);


        int rowLineUpperLeftX = 0;
        int rowLineUpperLeftY = UiPanes[paneIndex].freeRowLines[rowLineIndex].PixelCoordinateY - UiPanes[paneIndex].freeRowLines[rowLineIndex].RowLineBoundingHeight;
        

        int rowLineBottomRightX = renderTargetWidth;
        int rowLineBottomRightY = UiPanes[paneIndex].freeRowLines[rowLineIndex].PixelCoordinateY;
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineUpperLeftX : %d \n",rowLineUpperLeftX);
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineUpperLeftY : %d \n",rowLineUpperLeftY);
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineBottomRightX : %d \n",rowLineBottomRightX);
         //printf("DEBUGGING isUpdateBlockInRowLine : rowLineBottomRightY : %d \n",rowLineBottomRightY);
        
        /*
        if(((rowLineUpperLeftX <= inputUpdateBlockBottomRightPixelX) && (rowLineBottomRightX >= inputUpdateBlock.topLeftPixelCoordinateX)) && ((rowLineUpperLeftY <= inputUpdateBlockBottomRightPixelY )&&(rowLineBottomRightY >= inputUpdateBlock.topLeftPixelCoordinateY)) ){
            return true;
        }
        else{
            return false;
        }
        */

        return isRectangleInRectangle(inputUpdateBlock.topLeftPixelCoordinateX, inputUpdateBlock.topLeftPixelCoordinateY, inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width, inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height,
                               rowLineUpperLeftX,rowLineUpperLeftY,rowLineBottomRightX,rowLineBottomRightY);

        



    }

    



    

    bool isUpdateBlockInRowLineRegion(updateBlock inputUpdateBlock,int paneIndex,int rowLineIndex,updateBlock* returnOverlappingRegionUpdateBlock = 0){
        bool isOverlapping = 0;

        int inputUpdateBlockBottomRightPixelX = inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width;
        int inputUpdateBlockBottomRightPixelY = inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height;

        
        int rowLineRegionUpperLeftX = UiPanes[paneIndex].RowLineRegion->LeftSideXValue;
        int rowLineRegionUpperLeftY = UiPanes[paneIndex].RowLineRegion->TopSideYValue;//RightSideXValue;
        int rowLineRegionBottomRightX = UiPanes[paneIndex].RowLineRegion->RightSideXValue ;//;
        int rowLineRegionBottomRightY = UiPanes[paneIndex].RowLineRegion->BottomSideYValue ;

        if(((rowLineRegionUpperLeftX <= inputUpdateBlockBottomRightPixelX) && (rowLineRegionBottomRightX >= inputUpdateBlock.topLeftPixelCoordinateX)) && ((rowLineRegionUpperLeftY <= inputUpdateBlockBottomRightPixelY )&&(rowLineRegionBottomRightY >= inputUpdateBlock.topLeftPixelCoordinateY)) ){
            //printf("returning TRUE \n");
            isOverlapping = 1;
        }
        else{
            //printf("returning False \n");
            isOverlapping = 0;
            return false;
        }

        if(returnOverlappingRegionUpdateBlock != 0){
            int overlapRegionTopLeftX = returnMax(rowLineRegionUpperLeftX,inputUpdateBlock.topLeftPixelCoordinateX);
            int overlapRegionTopLeftY = returnMax(rowLineRegionUpperLeftY,inputUpdateBlock.topLeftPixelCoordinateY);
            int overlapRegionBottomRightX = returnMin(rowLineRegionBottomRightX,inputUpdateBlockBottomRightPixelX);
            int overlapRegionBottomRightY = returnMin(rowLineRegionBottomRightY,inputUpdateBlockBottomRightPixelY);

            int overlapRegionWidth = overlapRegionBottomRightX-overlapRegionTopLeftX;
            int overlapRegionHeight = overlapRegionBottomRightY - overlapRegionTopLeftY;

            returnOverlappingRegionUpdateBlock->ID = inputUpdateBlock.ID;
            returnOverlappingRegionUpdateBlock->width = overlapRegionWidth;
            returnOverlappingRegionUpdateBlock->height = overlapRegionHeight;
            returnOverlappingRegionUpdateBlock->Layer = inputUpdateBlock.Layer;
            returnOverlappingRegionUpdateBlock->pixelIndex = inputUpdateBlock.pixelIndex;
            returnOverlappingRegionUpdateBlock->topLeftPixelCoordinateX = overlapRegionTopLeftX;
            returnOverlappingRegionUpdateBlock->topLeftPixelCoordinateY = overlapRegionTopLeftY;
        }

        return isOverlapping;



    }


    int isUpdateBlockInRowLineRegionScrollBar(updateBlock inputUpdateBlock,scrollBar* scrollBarElement, bool horizontalOrVerticalScrollBar,updateBlock* scrollBarRegionOverlap, updateBlock* scrollBarSliderOverlap){

        if(horizontalOrVerticalScrollBar){

            //updateBlock scrollBarRegionOverLap = {};
            int returnVal1 = isRectangleInRectangle(inputUpdateBlock.topLeftPixelCoordinateX, inputUpdateBlock.topLeftPixelCoordinateY, inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width, inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height,
                                   scrollBarElement->ScrollableRegionTopLeftPixelX,scrollBarElement->ScrollableRegionTopLeftPixelY,scrollBarElement->ScrollableRegionBottomRightPixelX,scrollBarElement->ScrollableRegionBottomRightPixelY,
                                    scrollBarRegionOverlap);

            //updateBlock scrollBarSliderOverlap = {};
            int returnVal2 = isRectangleInRectangle(inputUpdateBlock.topLeftPixelCoordinateX, inputUpdateBlock.topLeftPixelCoordinateY, inputUpdateBlock.topLeftPixelCoordinateX + inputUpdateBlock.width, inputUpdateBlock.topLeftPixelCoordinateY + inputUpdateBlock.height,
                                   scrollBarElement->ScrollableRegionTopLeftPixelX, scrollBarElement->ScrollBarStartOfSliderPosition, scrollBarElement->ScrollableRegionBottomRightPixelX, scrollBarElement->ScrollBarEndOfSliderPosition,
                                   scrollBarSliderOverlap);

            //printf("returnVal1 = %d returnVal2 = %d \n", returnVal1, returnVal2);
            //isRectangleInRectangle() getOverlap region for scrollbar region
            //isRectangleInRectangle() getOverlap region for scrollbar slider
            return returnVal1 + returnVal2;
        }



        
    }
    



    int findUiMouseLocation2(int flag){
        //printf("uiData : findUiMouseLocation2() : \n");
        

        if(flag == 0){
            uiMouse.locationData[0][0] = -1;
            uiMouse.locationData[0][1] = -1;
            uiMouse.locationData[0][2] = -1;
            uiMouse.locationData[0][3] = -1;
            //1.) find highest layered pane that overlaps mouse, if no overlap set paneID to -1, return
            //2.) determine if the mouse is in a row line region, vertical scroll bar, horizontal scroll bar, or free bounding box
            //3.) IF the mouse is in row line region, determine if in cluster and retrieve index
            //4.) If the mosue is in cluster, determine if in uielement and retrieve index




            //1
            int highestLayerFound = -1;
            for(int i = 0; i < NumberOfPanes; i++){
                //if(isPointInRectangle(uiMouse.mouseXY.mousePixelCoordinateX,  uiMouse.mouseXY.mousePixelCoordinateY, UiPanes[i].LeftSideXValue, UiPanes[i].TopSideYValue,  UiPanes[i].RightSideXValue, UiPanes[i].BottomSideYValue ) && (UiPanes[i].Layer > highestLayerFound)){
                //    uiMouse.locationData[0][0] = i;
                //    highestLayerFound = UiPanes[i].Layer;
               // }
                if(UiPanes[i].findMouseLocation(&uiMouse) && (UiPanes[i].Layer > highestLayerFound)){
                    //printf("findUiMouseLocation2() : i = %d, UiPanes[i].PaneID = %d \n",i,UiPanes[i].PaneID);
                    uiMouse.locationData[0][0] = i;
                    uiMouse.locationDepth[0] = 0;
                    highestLayerFound = UiPanes[i].Layer;

               }

            }
            //printf("GETTING HERE YET? uiMouse.locationData[0][0] = %d \n",uiMouse.locationData[0][0]);
            if(uiMouse.locationData[0][0] == -1){return -1;}



            //2, 3, and 4
            
            //printf("TEASTINg 7.6.25 STOP 1 \n");
            //check if mouse is on border of current pane location
            if(UiPanes[uiMouse.locationData[0][0]].findMouseLocation(&uiMouse,3)){
                //printf("TEASTINg 7.6.25 STOP 2 \n");
                return 1235;
            }
            return UiPanes[uiMouse.locationData[0][0]].findMouseLocation(&uiMouse,2);
            
                

        }

        
        else if(flag == 1){
            //early out fallback if mouse was not previously in any pane
            if(uiMouse.locationData[1][0] == -1){
                return findUiMouseLocation2(0);
            }


            //if previous location was in rowlinecluster
            if(uiMouse.locationDepth[1] >= 2 && uiMouse.locationData[1][1] == 0){
                if(UiPanes[uiMouse.locationData[1][0]].RowLineRegion->RowLineClusters[uiMouse.locationData[1][2]].findMouseLocation(&uiMouse)){return 3;}
                uiMouse.locationDepth[1] = 1; //this will make us next test to see if its still in the same rowlineregion, since it is no longer in a rowLineCluster
            }


            //if previous location was in free bounding box
            if(uiMouse.locationDepth[1] == 2 && uiMouse.locationData[1][1] == 3){
                //Free Bounding boxes
                if(UiPanes[uiMouse.locationData[1][0]].findMouseLocation(&uiMouse,1)){return 2;}
                uiMouse.locationDepth[1] = 0;
            }


            //if previous location was in scrollbar
            if(uiMouse.locationDepth[1] == 2 && (uiMouse.locationData[1][1] == 1 || uiMouse.locationData[1][1] == 2)){
                if(UiPanes[uiMouse.locationData[1][0]].Scrollbars[uiMouse.locationData[1][1] - 1].findMouseLocation(&uiMouse)){return 2;}
                uiMouse.locationDepth[1] = 0;
            }


            //if previous location was in rowlineregion
            if(uiMouse.locationDepth[1] == 1){
                if(UiPanes[uiMouse.locationData[1][0]].RowLineRegion->findMouseLocation(&uiMouse)){return 2;}
                uiMouse.locationDepth[1] = uiMouse.locationDepth[1] - 1;
            }

            

            //if previous location was in pane 
            if(uiMouse.locationDepth[1] == 0){
                //printf("THIS ISNT HITTING IS IT??? \n");
                if(UiPanes[uiMouse.locationData[1][0]].findMouseLocation(&uiMouse,2)){return 1;}
            }
            else{
                findUiMouseLocation2(0);
            }

            //UiPanes[uiMouse.locationData[1][0]].RowLineRegion->HorizontalScrollBar->isMouseHovering()
        }
    }




    void sendUpdateBlock(int numberOfUpdateBlocks, updateBlock* updateBlocks){
        //printf("UI DATA: SENDING UPDATE BLOCK \n");
        
        if(*UpdateBlockQueueIndex >= (UpdateBlockQueueBufferSize - 1)){printf("UI ERROR UI ERROR : : UPDATEBLOCKBUFFEROVERFLOW : : \n"); return;}
        if((numberOfUpdateBlocks != 0) && (updateBlocks != nullptr)){
            for(int i = 0; i < numberOfUpdateBlocks; i++){
                UpdateBlockQueue[(*UpdateBlockQueueIndex)+i] = updateBlocks[i];
            }
        }

        *UpdateBlockQueueIndex = *UpdateBlockQueueIndex + numberOfUpdateBlocks;
        
    } 

    uiData(int uiRenderTargetWidth,int uiRenderTargetHeight,int uiPanesBufferSize, int keyPressRecordBufferSize, int updateBlockQueueBufferSize,int* numberOfUpdateBlocksPointer,updateBlock* updateBlockQueuePointer){
        if(keyPressRecordBufferSize < 4){
            //ERROR
        }
        
        
        UiPanes = (uiPane*)malloc(uiPanesBufferSize * sizeof(uiPane));
        UpdateBlockQueue = updateBlockQueuePointer;
        
        
        UpdateBlockQueueIndex = numberOfUpdateBlocksPointer;
        *UpdateBlockQueueIndex = 0;

        NumberOfPanes = 0;
        TotalNumberOfPixelsInDispatch = (int*)malloc(updateBlockQueueBufferSize * sizeof(int));
        memset(TotalNumberOfPixelsInDispatch,0,updateBlockQueueBufferSize * sizeof(int));

        DispatchStartIndices = (int*)malloc(updateBlockQueueBufferSize * sizeof(int));
        memset(DispatchStartIndices,0,updateBlockQueueBufferSize);

        DispatchLastIndices = (int*)malloc(updateBlockQueueBufferSize * sizeof(int));
        memset(DispatchLastIndices,0,updateBlockQueueBufferSize);

        

        UiPanesBufferSize = uiPanesBufferSize;
        UpdateBlockQueueBufferSize = updateBlockQueueBufferSize;

        uiMouse.initializeMouse(renderTargetWidth/2,renderTargetHeight/2);

        KeyPressRecordBufferSize = keyPressRecordBufferSize;
        KeyPressRecordStores = (KeyPressRecord*)malloc(4 * KeyPressRecordBufferSize * sizeof(KeyPressRecord));
        KeyPressRecordList = (KeyPressRecord*)malloc(KeyPressRecordBufferSize * sizeof(KeyPressRecord));

        firstProcessUpdate = 1;
        
        
    }

    void createPane(int id,int layer,int resizeableHorizontalL,int resizeableHorizontalR,int resizeableVerticalT,int resizeableVerticalB, int leftSide, int rightSide, int topSide, int bottomSide,int numberOfFreeBoundingBoxes){
        printf("UI DATA CREATING PANE... \n");
        UiPanes[NumberOfPanes] = uiPane(id,layer,resizeableHorizontalL,resizeableHorizontalR,resizeableVerticalT,resizeableVerticalB,leftSide,rightSide,topSide,bottomSide,numberOfFreeBoundingBoxes);
        printf("Uipanes[%d] = UipaneID %d \n",NumberOfPanes,UiPanes[NumberOfPanes].PaneID);
        NumberOfPanes = NumberOfPanes + 1;
    }





    void mergeSortUpdateBlocks_Merge(int first, int middle, int last){
        int leftSize = middle - first + 1;
        int rightSize = last - middle;

        updateBlock* left = (updateBlock*)malloc(sizeof(updateBlock) * (leftSize + 1));
        updateBlock* right = (updateBlock*)malloc(sizeof(updateBlock) * (rightSize + 1));

        for(int i = 0; i < leftSize; i++){
            left[i] = UpdateBlockQueue[first+i];
        }
        for(int i = 0; i < rightSize; i++){
            right[i] = UpdateBlockQueue[middle + i + 1];
        }

        updateBlock sentinelUpdateBlock = {};
        sentinelUpdateBlock.ID = 2147483647;
        sentinelUpdateBlock.Layer = 2147483647;

        left[leftSize] = sentinelUpdateBlock;
        right[rightSize] = sentinelUpdateBlock;

        for(int i = 0, j =0, k = first; k <= last; k++){
            if(left[i].Layer <= right[j].Layer){
                UpdateBlockQueue[k] = left[i];
                i++;
            }
            else{
                UpdateBlockQueue[k] = right[j];
                j++;
            }



        }
        free(left);
        free(right);

    }
    void mergeSortUpdateBlocks(int first, int last){
        if(first < last){
            int middle = (first + last) / 2;
            mergeSortUpdateBlocks(first,middle);
            mergeSortUpdateBlocks(middle+1,last);
            mergeSortUpdateBlocks_Merge(first,middle,last);

        }

    }

    //ONLY call this once the UpdateBlockQueue is sorted by layer
    void updateNumberOfDispatches(){
        printf("void updateNumberOfDispatches() : \n");
        int previousLayer = -9999;
        int numberOfLayers = 0;


        printf("void updateNumberOfDispatches() : (*UpdateBlockQueueIndex) = %d \n",(*UpdateBlockQueueIndex));
        for(int i = 0,j=0; i < (*UpdateBlockQueueIndex); i++){
            printf("void updateNumberOfDispatches() : for LOOP A{} : i = %d : j = %d : UpdateBlockQueue[i].Layer = %d : previousLayer = %d : \n",i,j,UpdateBlockQueue[i].Layer,previousLayer);
            if(UpdateBlockQueue[i].Layer != previousLayer){
                printf("void updateNumberOfDispatches() : for LOOP A{} : if{} : numberOfLayers = %d \n",numberOfLayers);
                numberOfLayers = numberOfLayers + 1;
                previousLayer = UpdateBlockQueue[i].Layer;
                DispatchStartIndices[j] = i;
                printf("void updateNumberOfDispatches() : for LOOP A{} : if{} : numberOfLayers = %d : previousLayer = %d : DispatchStartIndices[j] = %d \n",numberOfLayers,previousLayer,DispatchStartIndices[j]);
                j = j + 1;
                
            }

        }

        NumberOfDispatches = numberOfLayers;
        printf("void updateNumberOfDispatches() : NumberOfDispatches = %d \n",NumberOfDispatches);

        for(int i = 0; i < NumberOfDispatches; i++){
            printf("void updateNumberOfDispatches() : for LOOP B{} : i = %d \n",i);
            if(i == (NumberOfDispatches -1)){
                printf("void updateNumberOfDispatches() : for LOOP B{} : i = %d : if(i == (NumberOfDispatches -1)){} : \n",i);
                DispatchLastIndices[i] = (*UpdateBlockQueueIndex) - 1;
                printf("void updateNumberOfDispatches() : for LOOP B{} : i = %d : if(i == (NumberOfDispatches -1)){} : DispatchLastIndices[i] = %d \n",i);
            }
            else{
                printf("void updateNumberOfDispatches() : for LOOP B{} : i = %d : else{} : \n",i);
                DispatchLastIndices[i] = DispatchStartIndices[i+1] - 1;
                printf("void updateNumberOfDispatches() : for LOOP B{} : i = %d : else{} : DispatchLastIndices[i] = %d \n",i,DispatchLastIndices[i]);

            }


        }
       
    }


 

    void processMouseLocationChange2(){
    printf("processMouseLocationChange2() : \n");
    bool skipCurr = 0;
    //bool
    //If the left mouse button is held down AND the mouseLocation has not changed AND (mouseLocation is in a rowlineRegion's uiElement OR is in a freeBoundingBox)
    //Then only "click and drag" needs considered
    //If the left mouse button is held down AND the mousePosition has not changed Then nothing needs to be considered
    //skipcond3 = IF: in pane AND NOT in (freebounding box OR scrollbar) THEN: click and drag case
    //skipCond3   (If the value is 1 it is in rowlineregion but NOT in valid element then this is the multiselect drag case)
    //skipCond3   (If the value is 2 it is in a rowlineregion's valid element, and this is the click and drag case )
    int skipCond3 = 2 * (uiMouse.leftClickHeldLastFrame && !uiMouse.isSamePosition() && (uiMouse.initialMouseClickLocation[1] == 0)) + (uiMouse.initialMouseClickLocation[3] >= 0);
    bool skipCond2 = (uiMouse.leftClickHeldLastFrame && uiMouse.isSamePosition()); //&& ((uiMouse.locationData[0][1] == 0 && uiMouse.locationData[0][3] >= 0) || (uiMouse.locationData[0][1] == 3)));
    bool skipCond1 = (((uiMouse.currentContextLocation[1] == -1) && (uiMouse.locationData[0][0] == -1))) || (uiMouse.locationData[0][1] < -1);
    //skipCond4 skips the click and drag pathway if it has already been activated
    bool skipCond4 = uiMouse.cursorContext || ((skipCond3 == 2) && UiPanes[uiMouse.currentContextLocation[0]].RowLineRegion->isMouseMultiSelectActive);
    printf("processMouseLocationChange2() : skipCond1 = %d : skipCond2 = %d \n", skipCond1, skipCond2);
    printf("uiMouse.mouseKeys.buttons[0] = %d \n", uiMouse.mouseKeys.buttons[0]);
    if(uiMouse.mouseKeys.buttons[0] && !skipCond1 && !skipCond2 && !(skipCond3 >1)){
        uiMouse.initialMouseClickXY.mousePixelCoordinateX = uiMouse.mouseXY.mousePixelCoordinateX;
        uiMouse.initialMouseClickXY.mousePixelCoordinateY = uiMouse.mouseXY.mousePixelCoordinateY;

        if(uiMouse.leftClickHeldLastFrame){
            printf("if(uiMouse.leftClickHeldLastFrame) {} \n");

        }


        int newContext = 0;
        if(((uiMouse.currentContextLocation[0] != uiMouse.locationData[0][0]) || (uiMouse.currentContextLocation[1] != uiMouse.locationData[0][1])) ){
            newContext = 1 + ((uiMouse.currentContextLocation[0] != uiMouse.locationData[0][0])); 
        }
        printf("processMouseLocationChange2() : newContext = %d \n", newContext);
        printf("(uiMouse.locationData[0][1] != 1) = %d \n", (uiMouse.locationData[0][1] != 1));
        printf("(uiMouse.locationData[0][1] != 2)= %d \n", (uiMouse.locationData[0][1] != 2));
        if((newContext >= 1) && ((uiMouse.locationData[0][1] != 1) && (uiMouse.locationData[0][1] != 2))){
            //if the context switched and the old context had elements that were mouse selected
            if((uiMouse.currentContextLocation[1] == 0) && UiPanes[uiMouse.currentContextLocation[0]].RowLineRegion->hasMouseSelection()){UiPanes[uiMouse.currentContextLocation[0]].RowLineRegion->modifyMouseSelections(1); UiPanes[uiMouse.currentContextLocation[0]].NeedsRefresh[1] = 1;} //UiPanes[uiMouse.currentContextLocation[0]].NeedsRefresh[1] is used since this refresh pertains to rowlineregion
            uiMouse.currentContextLocation[0] = uiMouse.locationData[0][0]; 
            uiMouse.currentContextLocation[1] = uiMouse.locationData[0][1];
        }



        if(uiMouse.locationData[0][1] == 0){
            printf("processMouseLocationChange2() : if(uiMouse.locationData[0][1] == 0) \n");
            bool isInValidElement = uiMouse.locationData[0][3] >= 0;
            //if mouse is currently in a rowlineregion Element
            //if the mouse already has a selection in this rowlineregion, and ctrl is not held down
            if((uiMouse.currentContextLocation[1] == 0) && (!uiKeyboard.isCtrlHeld() ||(newContext >= 1) || !isInValidElement) && UiPanes[uiMouse.currentContextLocation[0]].RowLineRegion->hasMouseSelection()){printf("DEBUG (2 * ((newContext >= 1) && isInValidElement)) = %d",(2 * ((newContext >= 1) && isInValidElement)));UiPanes[uiMouse.currentContextLocation[0]].RowLineRegion->modifyMouseSelections((2 * ((newContext >= 1) && isInValidElement))); UiPanes[uiMouse.currentContextLocation[0]].NeedsRefresh[1] = 1;}

            
            //note how this uses short circuiting
            if(isInValidElement && (UiPanes[uiMouse.currentContextLocation[0]].RowLineRegion->selectElement(uiMouse.locationData[0][2],uiMouse.locationData[0][3]) == 0)){ UiPanes[uiMouse.currentContextLocation[0]].NeedsRefresh[1] = 1; }
        
            skipCurr = 1;
        }
        if(uiMouse.locationData[0][1] == 1){ //vertical scrollbar
            UiPanes[uiMouse.locationData[0][0]].Scrollbars[0].highLighted = 2;
            UiPanes[uiMouse.locationData[0][0]].Scrollbars[0].needsRefresh = 1;
            UiPanes[uiMouse.locationData[0][0]].NeedsRefresh[2] = 1;


        }
        if(uiMouse.locationData[0][1] == 2){ //horizontal scrollbar

        }
        if(uiMouse.locationData[0][1] == 3){//free bounding box

        }

        uiMouse.leftClickHeldLastFrame = 1;
    }
    else if((!skipCond4 && uiMouse.mouseKeys.buttons[0] && (skipCond3>1)) || (skipCond4 && !uiMouse.mouseKeys.buttons[0])){
        
        
        //1.)  if click and drag is in rowlineregion but not valid element
        //1.A) setMouseCursorContext() to multiselect drag, this mode causes the rowlineregion create and resize a "selectregion" according to where mouse is now and
        //     and where the intiial mouse click occured
        if(skipCond3 == 2){
            UiPanes[uiMouse.locationData[0][0]].RowLineRegion->setMultiSelect(uiMouse.mouseKeys.buttons[0]);
        }  

         //2.)  if the click and drag was on a uiElement
        //2.A) check to see if mouse has moved far enough from target click
        //2.B) if it has then setMouseCursorContext() to click and drag element, attach the uiElementID of the element that is clicked on to the mouse cursor
        if(skipCond3 == 3){
            double distanceFromInitialClick = 0;//((uiMouse.mouseXY.mousePixelCoordinateX - uiMouse.initialMouseClickXY.mousePixelCoordinateX) ^ 2) + (uiMouse.mouseXY.mousePixelCoordinateY - uiMouse.initialMouseClickXY.mousePixelCoordinateY)
            if(distanceFromInitialClick > 5){
                uiMouse.setMouseCursorContext(skipCond3);
                skipCond4 = 1;
            }
        }


        skipCond4 = !skipCond4; //first pass turns 0 to 1, second pass turns 1 to 0
    }
    else if((uiMouse.leftClickHeldLastFrame == 1) && !uiMouse.mouseKeys.buttons[0]){
        uiMouse.leftClickHeldLastFrame = 0;
    }

    

    if(uiMouse.isSameLocation(2)){ printf("processMouseLocationChange2() : EARLY EXIT CONDITION \n");return;}
    
        
        


    printf("\t processMouseLocationChange2 : entering for loop \n");

    for (int currOrPrev = 1 - firstProcessUpdate; currOrPrev >= 0; currOrPrev--) {
        //printf("\t processMouseLocationChange2 : forloop{} : currOrPrev = %d \n",currOrPrev);
        int paneIndex = uiMouse.locationData[currOrPrev][0];
        int type = uiMouse.locationData[currOrPrev][1];
        int clusterIndex = uiMouse.locationData[currOrPrev][2];
        int elementIndex = uiMouse.locationData[currOrPrev][3];
        int highlight = (currOrPrev == 0);
        printf("currOrPrev = %d , paneIndex = %d, type = %d, clusterIndex = %d, elementIndex = %d, highlight = %d \n",
                currOrPrev,paneIndex,type,clusterIndex,elementIndex,highlight);

       
        
        if(type == -1 || (type == 0 && elementIndex < 0) || ((currOrPrev == 0) && skipCurr)){
            printf("if(type == -1 || (type == 0 && elementIndex < 0) || ((currOrPrev == 0) && skipCurr)){} \n");
           
            continue;
        }

        if (type == 0 && clusterIndex >=0 && elementIndex >=0) { //RowLineRegion (UI element)


            int clusterRowLineStartIndex = UiPanes[paneIndex].RowLineRegion->RowLineClusters[clusterIndex].StartingRowLineIndex;
            //printf("\t processMouseLocationChange2 : forloop{} : if((type == 0 && clusterIndex >=0 && elementIndex >=0)) == TRUE \n");
            //printf("\t processMouseLocationChange2 : forloop{} : if() == TRUE : clusterRowLineStartIndex = %d \n",clusterRowLineStartIndex);
            printf("if (type == 0 && clusterIndex >=0 && elementIndex >=0){} , clusterRowLineStartIndex = %d \n", clusterRowLineStartIndex);
           
            //UiPanes[paneIndex].RowLineRegion->RowLineClusters[clusterIndex].highlightElement(elementIndex,highlight);
            UiPanes[paneIndex].RowLineRegion->needsRefresh[0] = 2;
            UiPanes[paneIndex].RowLineRegion->needsRefresh[3] = 1; 
            UiPanes[paneIndex].RowLineRegion->needsRefresh[4] = 1; 
            UiPanes[paneIndex].NeedsRefresh[1] = 1;                        
        }

        else if ((type == 1 || type == 2) && UiPanes[paneIndex].Scrollbars[type-1].highLighted != 2) {// Vertical Scrollbar
            //printf("\t processMouseLocationChange2 : forloop{} : if((type == 1 || type == 2)) == TRUE \n");
            UiPanes[paneIndex].Scrollbars[type-1].needsRefresh = 1;
            UiPanes[paneIndex].NeedsRefresh[2] = 1;
            UiPanes[paneIndex].Scrollbars[type-1].highLighted = highlight;
        }
        else if(type == 3){
            //printf("\t processMouseLocationChange2 : forloop{} : if((type == 3)) == TRUE \n");
            for(int i = 0; i < UiPanes[paneIndex].FreeBoundingBoxes[uiMouse.locationData[currOrPrev][2]].NumberOfFreeRowLinesToTrigger; i++){
                int freeRowLineIndex = UiPanes[paneIndex].FreeBoundingBoxes[uiMouse.locationData[currOrPrev][2]].TriggerFreeRowLineIndices[i];
                int numberOfFreeRowLineElementsToTrigger = UiPanes[paneIndex].FreeBoundingBoxes[uiMouse.locationData[currOrPrev][2]].NumberOfFreeRowLineElementsToTrigger[i];

                if(UiPanes[paneIndex].freeRowLines[freeRowLineIndex].VisualElementBuffer){
                    //printf("if(UiPanes[uiMouse.mouseUi_CurrentLocation_PaneIndex].freeRowLines[freeRowLineIndex].uiElementBuffer) = true \n",UiPanes[paneIndex].freeRowLines[freeRowLineIndex].uiElementBuffer);
                    for(int j = 0; j < numberOfFreeRowLineElementsToTrigger; j++){
                        int elementIndex = UiPanes[paneIndex].FreeBoundingBoxes[uiMouse.locationData[currOrPrev][2]].TriggerFreeElementIndices[i][j];
                        //printf("uiElementBuffer : int elementIndex = UiPanes[uiMouse.mouseUi_CurrentLocation_PaneIndex].FreeBoundingBoxes[uiMouse.mouseUi_CurrentLocation_FreeBoundingBoxIndex].TriggerFreeElementIndices[i][j]= %d \n",elementIndex);
                        UiPanes[paneIndex].freeRowLines[freeRowLineIndex].VisualElementBuffer[elementIndex].highlighted =  highlight;
                        UiPanes[paneIndex].freeRowLines[freeRowLineIndex].needsRefresh = 1;
                        UiPanes[paneIndex].freeRowLinesNeedRefresh = 3;
                        UiPanes[paneIndex].NeedsRefresh[3] = 1;
                    }


                }
                else if(UiPanes[paneIndex].freeRowLines[freeRowLineIndex].TextBoxBuffer){
                    for(int j = 0; j < numberOfFreeRowLineElementsToTrigger; j++){
                        int elementIndex = UiPanes[paneIndex].FreeBoundingBoxes[uiMouse.locationData[currOrPrev][2]].TriggerFreeElementIndices[i][j];
                        UiPanes[paneIndex].freeRowLines[freeRowLineIndex].TextBoxBuffer[elementIndex].highlighted =  highlight;
                        UiPanes[paneIndex].freeRowLines[freeRowLineIndex].needsRefresh = 1;
                        UiPanes[paneIndex].freeRowLinesNeedRefresh = 3;
                        UiPanes[paneIndex].NeedsRefresh[3] = 1;
                    }

                }
            }
        }
        
        
    }


}



    //returns false when the keyCode is NOT a modifier key, and true if the keycode IS a modifier key
    bool updateModifierStateOnPress(int keyCode){
        if(keyCode == KEY_LEFTSHIFT || keyCode == KEY_RIGHTSHIFT) {
            uiMainController.shiftHeld = 1;
            return true;
        }
        else if(keyCode == KEY_LEFTCTRL || keyCode == KEY_RIGHTCTRL) {
            uiMainController.ctrlHeld = 1;
            return true;
        }
        else if(keyCode == KEY_LEFTALT || keyCode == KEY_RIGHTALT) {
            uiMainController.altHeld = 1;
            return true;
        }
        else if(keyCode == KEY_CAPSLOCK){
            uiMainController.capsLockToggled = !(uiMainController.capsLockToggled);
            return true;
        }
        return false;
    }

//denote leftShift as LS
//lets say we repeat key every y seconds
//received publication with {,.... (LS,S0,-D0), ('t',S1,D1) }

// D0 = -2y. D1 = 3y

// (D1 - S1 / y) = numberOfTimesToEmitKey
// for(double time = S1; time <= D1; time= time + y){}
//      emitKey(); and if modiiferKeyActive || time < modifierKeyEndTime

//i think at the end of pass, we reset modifierKeyEndTime to be a INVALID_SENTINEL value





//another example
//received publication with {,.... (LS,S0,D0), ('t',S1,D1) }
//in this case you can be sure that every recordedKeyPress should have the modifier state






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                          another example
//received publication with {(LS,S0,D0_0), } so modifier state is ON, and its end time is set to CONTINUING_SENTINEL
//and then
//received publication with { ('t',s1,D1_0), (LS,S0,D0_1) } so modifier is still ON and its end time remains as CONTINUING_SENTINEL
//and then
//received publication with { (LS,S0,-D0_2), ('t',s1,D1_1) } so modifier is turned OFF and its end time is set as D0_2
//and then
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                          another example *Problem found in design, how would the last 't' record know that leftShift has turned off?*
//received publication with {(LS,S0,D0_0), } so modifier state is ON, and its end time is set to CONTINUING_SENTINEL
//and then
//received publication with { (LS,S0,D0_1), ('t',s1,D1_0) } so modifier is still ON and its end time remains as CONTINUING_SENTINEL
//and then
//received publication with {('t',s1,D1_1),  (LS,S0,-D0_2) } so modifier is turned OFF and its end time is set as D0_2
//and then
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





/*
perhaps we need to redo the input thread to now record entries as
('exampleKey',startTime,DeltaTime)

....Other key presses..........

('exampleKey',startTime,-DeltaTime)         <-------------------------- this signifies when 'exampleKey' was released


*/



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//if we included a startTime and EndTime for modifier keys on a controller state

    bool focusedTextBoxAcceptsText(){

    }

    char keyCodeToCharacterUsingCurrentModifiers(){

    }



    /*
    void processPublishedKeyRecords(KeyPressRecord* records, int numberOfRecords) {

        // Records are assumed sorted by original press/start order.
        for(int r = 0; r < numberOfRecords; r++) {

            int keyCode = records[r].keyCode;
            double dt = records[r].deltaTime;

            bool isOngoing = dt > 0.0;
            bool isReleased = dt < 0.0;

            
            
            if(isOngoing) {
                double currentDelta = dt;
                double previousDelta = uiMainController.keyboardKeysCurrentlyPressed_DeltaTiming[keyCode];

                // If main thread did not yet know this key was held,
                // this record represents a newly observed press.
                if(uiMainController.keyboardKeysCurrentlyPressed[keyCode] == 0) {
                    uiMainController.keyboardKeysCurrentlyPressed[keyCode] = 1;
                    uiMainController.keyboardKeysCurrentlyPressed_DeltaTiming[keyCode] = currentDelta;
                    
                  
                    bool isCurrentEntryModifierKey = updateModifierStateOnPress(keyCode);

                    if(focusedTextBoxAcceptsText()) {
                        char c = keyCodeToCharacterUsingCurrentModifiers(keyCode);
                        if(c != 0) {
                            focusedTextBoxAppendCharacter(c);
                        }
                    }

                    continue;
                }

                // Existing hold: consume only the new elapsed time.
                double deltaSinceLastMainFrame = currentDelta - previousDelta;
                if(deltaSinceLastMainFrame < 0.0) {
                    //CRASH?
                    //so if the input thread sends a publication of {'L',0.002 seconds} into slot 3
                    //main thread acquires slot 3
                    //NOTE: remember that the RecordedKeyPresses is NOT cleared by main thread with the payloads, thinking this would be to confuse two very different
                    //      things that are done for very different reasons. The RecordedKeyPresses is cleared by the input thread, and its done because the input thread
                    //      must clear/emoty its own local RecordedKeyPresses buffer. This is also why its important for the main thread to track its own state regarding 
                    //      received input (note 530 13:24 : is this technically true)
                    //
                    //     
                    //      
                    deltaSinceLastMainFrame = 0.0; // safety against reset/rebuild weirdness
                }

                //so i think in order to properly determine the number of emissions on a held key, we 
                //int numberOfEmissionsInCurrentHold[255]

                int repeatCount = calculateRepeatsCrossed(
                    previousDelta,
                    currentDelta,
                    repeatDelay,
                    repeatInterval
                );

                if(focusedTextBoxAcceptsText()) {
                    char c = keyCodeToCharacterUsingCurrentModifiers(keyCode);
                    for(int n = 0; n < repeatCount; n++) {
                        focusedTextBoxAppendCharacter(c);
                    }
                }

                mainKeyboard.lastConsumedDelta[keyCode] = currentDelta;
            }

            else if(isReleased) {
                double finalDelta = -dt;
                double previousDelta = mainKeyboard.lastConsumedDelta[keyCode];

                // Consume any repeat boundaries crossed between last seen held time
                // and final release time.
                if(mainKeyboard.keyHeld[keyCode]) {
                    int repeatCount = calculateRepeatsCrossed(
                        previousDelta,
                        finalDelta,
                        repeatDelay,
                        repeatInterval
                    );

                    if(focusedTextBoxAcceptsText()) {
                        char c = keyCodeToCharacterUsingCurrentModifiers(keyCode);
                        for(int n = 0; n < repeatCount; n++) {
                            focusedTextBoxAppendCharacter(c);
                        }
                    }
                }

                updateModifierStateOnRelease(keyCode);

                mainKeyboard.keyHeld[keyCode] = 0;
                mainKeyboard.lastConsumedDelta[keyCode] = 0.0;
                mainKeyboard.initialPressConsumed[keyCode] = false;
            }
        }
    }

    */


    //perhpas now processUpdates will need to return things such as objectSelectionRequests (from ui side panel), mouse gameview position, editor view or full runtime
 
    void processUpdates(){
        printf("\n\n\n\n\n\n\n\n\n\n\n\n processUpdates() : BEGIN DEBUGGING \n");
        
       
        //printf("processUpdates() : Beginning updating visible rowline indices  \n");
        for(int i = 0; i < NumberOfPanes; i++){
            if(UiPanes[i].RowLineRegion){
                UiPanes[i].RowLineRegion->updateVisibleIndices();
            }
        }
        //printf("processUpdates() : Finished updating visible rowline indices \n");
        //printf("processUpdates() : Beginning to change mouse location \n");
        //printf("processUpdates() : firstProcessUpdate = %d \n",firstProcessUpdate);
        if(firstProcessUpdate){
            //printf("processUpdates() : Changing Mouse Location : if(firstProcessUpdate) == true : will now invoke findUiMouseLocation(0)  \n");
            findUiMouseLocation2(0);
            //printf("so this is hitting too? \n");
            firstProcessUpdate = 0;
            //printf("processUpdates() : Changing Mouse Location : if(firstProcessUpdate) == true : findUiMouseLocation(0) invocation finished and firstProcessUpdate is set to 0 \n");

        }
        else{
                //printf("processUpdates() : Changing Mouse Location : if(firstProcessUpdate) == false : will now update previous locations  \n");
                uiMouse.previousMouseXY.mousePixelCoordinateX = uiMouse.mouseXY.mousePixelCoordinateX;
                uiMouse.previousMouseXY.mousePixelCoordinateY = uiMouse.mouseXY.mousePixelCoordinateY;

                uiMouse.PreviousCursorUpdateBlock = uiMouse.currentCursorUpdateBlock[0]; //mouseUpdate;
                uiMouse.PreviousCursorUpdateBlock.ID = 2147483646;

                uiMouse.locationData[1][0]=uiMouse.locationData[0][0];
                uiMouse.locationData[1][1]=uiMouse.locationData[0][1];
                uiMouse.locationData[1][2]=uiMouse.locationData[0][2];
                uiMouse.locationData[1][3]=uiMouse.locationData[0][3];
                //printf("processUpdates() : Changing Mouse Location : if(firstProcessUpdate) == false : finished updating previous locations  \n");
                //printf("processUpdates() : Changing Mouse Location : if(firstProcessUpdate) == false : will now invoke findUiMouseLocation(1)  \n");
                findUiMouseLocation2(0);
        }
        //printf("processUpdates() : Changing Mouse Location : if(firstProcessUpdate) == false : finished invoking findUiMouseLocation(1)  \n");
        //printf("processUpdates() : Changing Mouse Location : if(firstProcessUpdate) == false : Now reaching if-else branch  \n");
        //if previous mouse location and current mouse location are in the same pane
        printf("\n\n processUpdates() : uiMouse.locationData[1][0] = %d \n",uiMouse.locationData[1][0]);
        printf("processUpdates() : uiMouse.locationData[1][1] = %d \n",uiMouse.locationData[1][1]);
        printf("processUpdates() : uiMouse.locationData[1][2] = %d \n", uiMouse.locationData[1][2]);
        printf("processUpdates() : uiMouse.locationData[1][3] = %d \n", uiMouse.locationData[1][3]);
        printf("\n\n processUpdates() : uiMouse.locationData[0][0] = %d \n",uiMouse.locationData[0][0]);
        printf("processUpdates() : uiMouse.locationData[0][1] = %d \n",uiMouse.locationData[0][1]);
        printf("processUpdates() : uiMouse.locationData[0][2] = %d \n", uiMouse.locationData[0][2]);
        printf("processUpdates() : uiMouse.locationData[0][3] = %d \n", uiMouse.locationData[0][3]);

        printf("processUpdates() : uiMouse.currentContextLocation[0] = %d \n", uiMouse.currentContextLocation[0]);
        printf("processUpdates() : uiMouse.currentContextLocation[1] = %d \n", uiMouse.currentContextLocation[1]);

        for(int i = 0; i < NumberOfPanes; i++){
            if(UiPanes[i].RowLineRegion){
                for(int j = 0; j < UiPanes[i].RowLineRegion->NumberOfMouseSelectedElements * 2; j++){
                    printf("UiPanes[%d].RowlineRegion->mouseSelectedLocation[%d] = %d \n",i,j,UiPanes[i].RowLineRegion->mouseSelectedLocation[j]);

                }
            }
        }
        

        
        processMouseLocationChange2();     
                

        


        //printf("processUpdates() : Finshing changing mouse location \n");
        
        
       

 

        //printf("DEBUGGING processUpdates() 2\n");
        //printf("DEBUGGING processUpdates() : UiPanes[1].RowLineBuffer[0].TextBoxBuffer[0].Height) = %d \n", UiPanes[1].RowLineBuffer[0].TextBoxBuffer[0].Height);
        
        //refresh the screen from where previous cursor

        
        if(uiMouse.PreviousCursorUpdateBlock.ID == 2147483646){
            
            
            updateBlock paneOverlapRegion = {};
            for(int i = 0; i < NumberOfPanes; i ++){
                if(isUpdateBlockInPane(uiMouse.PreviousCursorUpdateBlock,i,&paneOverlapRegion)){
                    UiPanes[i].addRedrawRegion(paneOverlapRegion.topLeftPixelCoordinateX,paneOverlapRegion.topLeftPixelCoordinateY,paneOverlapRegion.topLeftPixelCoordinateX + paneOverlapRegion.width, paneOverlapRegion.topLeftPixelCoordinateY + paneOverlapRegion.height);
                    

                    if(UiPanes[i].Scrollbars){
                        //refresh scrollbar
                        if(UiPanes[i].Scrollbars[1].Type == 1){

                        }
                        if(UiPanes[i].Scrollbars[0].Type == 0){

                            updateBlock scrollBarRegionOverlapRegion = {};
                            updateBlock scrollBarSliderOverlapRegion = {};
                            if(isUpdateBlockInRowLineRegionScrollBar(uiMouse.PreviousCursorUpdateBlock,&(UiPanes[i].Scrollbars[0]),1,&scrollBarRegionOverlapRegion,&scrollBarSliderOverlapRegion)){
                                UiPanes[i].Scrollbars[0].addRedrawRegion(scrollBarRegionOverlapRegion.topLeftPixelCoordinateX, scrollBarRegionOverlapRegion.topLeftPixelCoordinateY, scrollBarRegionOverlapRegion.topLeftPixelCoordinateX + scrollBarRegionOverlapRegion.width, scrollBarRegionOverlapRegion.topLeftPixelCoordinateY + scrollBarRegionOverlapRegion.height,0);
                                UiPanes[i].Scrollbars[0].addRedrawRegion(scrollBarSliderOverlapRegion.topLeftPixelCoordinateX, scrollBarSliderOverlapRegion.topLeftPixelCoordinateY, scrollBarSliderOverlapRegion.topLeftPixelCoordinateX + scrollBarSliderOverlapRegion.width, scrollBarSliderOverlapRegion.topLeftPixelCoordinateY + scrollBarSliderOverlapRegion.height,1);  
                            }
                        }
                    }
                        
                        
                    if(UiPanes[i].RowLineRegion){
                        //refresh rowLineRegion
                        for(int j = UiPanes[i].RowLineRegion->FirstVisibleRowLineIndexTop; j <= UiPanes[i].RowLineRegion->LastVisibleRowLineIndexBottom; j++){
                            //printf("GOT HERE??? \n");
                            if(isUpdateBlockInRowLine(uiMouse.PreviousCursorUpdateBlock,i,j)){
                                //printf("GOT HERE \n");
                                

                                if(UiPanes[i].RowLineRegion->RowLineBuffer[j].TextBoxBuffer){
                                    //printf("GOT HERE! \n");
                                    for(int k = 0; k <  UiPanes[i].RowLineRegion->RowLineBuffer[j].TextBoxBufferIndex; k++){
                                        //int textUpdateIndicesSize = UiPanes[i].RowLineRegion->RowLineBuffer[j].TextBoxBufferIndex;
                                        int textUpdateIndicesSize = UiPanes[i].RowLineRegion->RowLineBuffer[j].TextBoxBuffer[k].TextBufferIndex;
                                        bool textUpdateIndices[textUpdateIndicesSize] = {0};
                                        if(UiPanes[i].RowLineRegion->RowLineBuffer[j].TextBoxBuffer[k].isUpdateBlockInTextBox(uiMouse.PreviousCursorUpdateBlock,textUpdateIndices)){
                                            UiPanes[i].RowLineRegion->RowLineBuffer[j].TextBoxBuffer[k].AddRedrawRegion(textUpdateIndices);
                                        }
                                    }
                                }
                                else if(UiPanes[i].RowLineRegion->RowLineBuffer[j].VisualElementBuffer){
                                    //printf("GOT HERE !! \n");
                                
                                    for(int k = 0; k < UiPanes[i].RowLineRegion->RowLineBuffer[j].VisualElementBufferIndex; k++){
                                        
                                        updateBlock uiElementOverlapRegionWithMouse = {};
                                        if(UiPanes[i].RowLineRegion->RowLineBuffer[j].VisualElementBuffer[k].isUpdateBlockInUiElement(uiMouse.PreviousCursorUpdateBlock,&uiElementOverlapRegionWithMouse)){
                                            updateBlock overlapRegionRefreshUiElement = {}; 
                                            isUpdateBlockInRowLineRegion(uiElementOverlapRegionWithMouse,i,j,&overlapRegionRefreshUiElement);
                                            if(UiPanes[i].RowLineRegion->RowLineBuffer[j].VisualElementBuffer[k].highlighted > 0){
                                                overlapRegionRefreshUiElement.ID = overlapRegionRefreshUiElement.ID + 1000 * UiPanes[i].RowLineRegion->RowLineBuffer[j].VisualElementBuffer[k].highlighted;
                                            }
                                            //sendUpdateBlock(1,&overlapRegionRefreshUiElement);
                                            UiPanes[i].RowLineRegion->RowLineBuffer[j].VisualElementBuffer[k].AddRedrawRegion();
                                          
                                        }
                                        
                                    }

                                }

                            }
                            
                        }
                    }
                    if(UiPanes[i].freeRowLines){
                        
                        //printf("DEBUG TEST HERE FOR NOW %d \n", UiPanes[i].numberOfFreeRowLines);
                        for(int m = 0; m < UiPanes[i].numberOfFreeRowLines; m++){
                           
                            
                            //printf("m = %d \n", m);
                            if(isUpdateBlockInFreeRowLine(uiMouse.PreviousCursorUpdateBlock,i,m)){
                                
                                
                                if(UiPanes[i].freeRowLines[m].TextBoxBuffer){
                                    //printf("DEBUGGING ALSO HIT HERE AS WELL 1 \n");
                                    for(int k = 0; k <  UiPanes[i].freeRowLines[m].TextBoxBufferIndex; k++){
                                        //printf("k = %d \n", k);
                                        //int textUpdateIndicesSize = UiPanes[i].freeRowLines[m].TextBoxBufferIndex;
                                        int textUpdateIndicesSize = UiPanes[i].freeRowLines[m].TextBoxBuffer[k].TextBufferIndex;
                                        bool textUpdateIndices[textUpdateIndicesSize] = {0};
                                        //printf("GOT HERE!\n");
                                        if(UiPanes[i].freeRowLines[m].TextBoxBuffer[k].isUpdateBlockInTextBox(uiMouse.PreviousCursorUpdateBlock,textUpdateIndices)){
                                            UiPanes[i].freeRowLines[m].TextBoxBuffer[k].AddRedrawRegion(textUpdateIndices);
                                        }
                                    }
                                }
                                else if(UiPanes[i].freeRowLines[m].VisualElementBuffer){
                                    //printf("DEBUGGING ALSO HIT HERE AS WELL 2 \n");
                                    for(int k = 0; k < UiPanes[i].freeRowLines[m].VisualElementBufferIndex; k++){
                                        updateBlock visualElementOverlapRegion = {};
                                        if(UiPanes[i].freeRowLines[m].VisualElementBuffer[k].isUpdateBlockInUiElement(uiMouse.PreviousCursorUpdateBlock,&visualElementOverlapRegion)){
                                            //updateBlock refreshUiElement = {};
                                            visualElementOverlapRegion.ID = UiPanes[i].freeRowLines[m].VisualElementBuffer[k].ElementID;
                                            
                                            visualElementOverlapRegion.Layer = UiPanes[i].freeRowLines[m].VisualElementBuffer[k].Layer;
                                            if(UiPanes[i].freeRowLines[m].VisualElementBuffer[k].highlighted == 1){
                                                visualElementOverlapRegion.ID = visualElementOverlapRegion.ID + 100;
                                            }
                                            
                                            UiPanes[i].freeRowLines[m].VisualElementBuffer[k].AddRedrawRegion();
                                        }
                                    }

                                }
                                

                            }
                        
                        }
                        

                    }

                }

                
            }


        }
        

        printf("DEBUGGING processUpdates() 3\n");


       
        printf("processUpdates() : Beginning to enter updateBlock submission stage :\n");
       for(int i = 0; i < NumberOfPanes; i++){ //i is each pane
            if(UiPanes[i].NeedsRefresh[0] > 0){
                printf("processUpdates() : UiPanes[%d].NeedsRefresh[0] = %d \n",i,UiPanes[i].NeedsRefresh[0]);
                if(UiPanes[i].NeedsRefresh[0] == 2){ // full pane refresh
                    updateBlock paneBackground = {};
                    paneBackground.ID = UiPanes[i].PaneID;
                    paneBackground.width = UiPanes[i].RightSideXValue - UiPanes[i].LeftSideXValue;
                    paneBackground.height = UiPanes[i].BottomSideYValue - UiPanes[i].TopSideYValue;
                    paneBackground.topLeftPixelCoordinateX = UiPanes[i].LeftSideXValue;
                    paneBackground.topLeftPixelCoordinateY = UiPanes[i].TopSideYValue;
                    paneBackground.Layer = UiPanes[i].Layer;
                    sendUpdateBlock(1,&paneBackground);
                }
                
                if(UiPanes[i].RowLineRegion){
                    //render scrollBars first
                    UiPanes[i].renderScrollBars(this,i);

                    
                    printf("processUpdates() : UiPanes[%d].RowLineRegion->needsRefresh[0] = %d, RowLineRegion->needsRefresh[1] = %d, RowLineRegion->needsRefresh[2] = %d, RowLineRegion->needsRefresh[3] = %d \n",
                          i,UiPanes[i].RowLineRegion->needsRefresh[0], UiPanes[i].RowLineRegion->needsRefresh[1], UiPanes[i].RowLineRegion->needsRefresh[2], UiPanes[i].RowLineRegion->needsRefresh[3]);

                    if(UiPanes[i].RowLineRegion->needsRefresh[0] == 1){ //FULL REFRESH
                        printf("if(UiPanes[i].RowLineRegion->needsRefresh[0] == 1) \n");
                        //render rowLineRegion
                        UiPanes[i].RowLineRegion->render(this);
                        UiPanes[i].RowLineRegion->needsRefresh[0] = 0 + UiPanes[i].RowLineRegion->needsRefresh[0] * isAnimated;

                    }

                   
                    else if(UiPanes[i].RowLineRegion->needsRefresh[0] == 2){ //PARTIAL REFRESH

                        

                        if(UiPanes[i].RowLineRegion->needsRefresh[2] == 1){ 
                            //printf("Debug 10/21/25 : if(UiPanes[i].RowLineRegion->needsRefresh[2] == 1) : UiPanes[i].RowLineRegion->NumberOfMouseSelectedElements = %d \n",UiPanes[i].RowLineRegion->NumberOfMouseSelectedElements);
                            bool mouseSelectionNeedsCleared = 0;
                            if(UiPanes[i].RowLineRegion->NumberOfMouseSelectedElements < 0){
                                mouseSelectionNeedsCleared = 1;
                                UiPanes[i].RowLineRegion->NumberOfMouseSelectedElements = -UiPanes[i].RowLineRegion->NumberOfMouseSelectedElements;

                            }
                            //printf("GOT HERER!!!!???!102425 \n");
                            for(int j = 0; j < UiPanes[i].RowLineRegion->NumberOfMouseSelectedElements * 2; j = j +2){
                                //printf("GOT HERER!!!!???!102425 j = %d \n", j);
                                UiPanes[i].RowLineRegion->renderSpecificElement(this,UiPanes[i].RowLineRegion->mouseSelectedLocation[j],UiPanes[i].RowLineRegion->mouseSelectedLocation[j+1]);
                            }
                            //printf("ORRR HERER!!!!???!102425 \n");
                            if(mouseSelectionNeedsCleared){UiPanes[i].RowLineRegion->modifyMouseSelections(-1 - (UiPanes[i].RowLineRegion->needsRefresh[1] == 1));}
                            
                            //printf("NO WAY ITS HERER!!!!???!102425 \n");
                            UiPanes[i].RowLineRegion->needsRefresh[2] = 0;
                        }
                        if(UiPanes[i].RowLineRegion->needsRefresh[1] == 1){
                            UiPanes[i].RowLineRegion->renderSpecificElement(this,uiMouse.locationData[0][2],uiMouse.locationData[0][3]);
                            UiPanes[i].RowLineRegion->needsRefresh[1] = 0;

                        }
                        
                        if(UiPanes[i].RowLineRegion->needsRefresh[3] > 0){ 
                            //printf("else if(UiPanes[i].RowLineRegion->needsRefresh[2] > 0){} \n");

                            //printf("process updates() : UiPanes[i].RowLineRegion->needsRefresh == 3 \n ");
                            
                            
                            if(uiMouse.locationData[0][3] >= 0){
                                UiPanes[i].RowLineRegion->renderSpecificElement(this,uiMouse.locationData[0][2],uiMouse.locationData[0][3]);
                                //printf("is this last \n");
                            }
                            UiPanes[i].RowLineRegion->needsRefresh[3] = 0;
                        }

                    
                        if(UiPanes[i].RowLineRegion->needsRefresh[4] > 0){ 
                            //printf("else if(UiPanes[i].RowLineRegion->needsRefresh[3] > 0){} \n");
                            if(uiMouse.locationData[1][3] >= 0){
                                    //printf("processUpdates() : this shouldnt hit yet : \n");
                                    UiPanes[i].RowLineRegion->renderSpecificElement(this,uiMouse.locationData[1][2],uiMouse.locationData[1][3]);
                                }
                                UiPanes[i].RowLineRegion->needsRefresh[4] = 0;
                        }

                        
                        UiPanes[i].RowLineRegion->needsRefresh[0] = 0;
                    }
                }
                //printf("10-08 all the way here? \n");
                if(UiPanes[i].freeRowLines){
                    if(UiPanes[i].freeRowLinesNeedRefresh == 1){
                        for(int j = 0; j < UiPanes[i].numberOfFreeRowLines;j++){
                            if(UiPanes[i].freeRowLines[j].needsRefresh){
                                UiPanes[i].freeRowLines[j].render(this,0);
                                UiPanes[i].freeRowLines[j].needsRefresh =  UiPanes[i].freeRowLines[j].needsRefresh * isAnimated;
                            }

                        }
                        UiPanes[i].freeRowLinesNeedRefresh = 0 + UiPanes[i].freeRowLinesNeedRefresh * isAnimated;
                    }
                    else if(UiPanes[i].freeRowLinesNeedRefresh == 3){
                        //notice how these two paths are essentially the exact same??
                        //why not just collapse them?
                        if(uiMouse.locationData[1][1] == 3 && uiMouse.locationData[1][2] >= 0){
                            for(int k = 0; k < UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[1][2]].NumberOfFreeRowLinesToTrigger; k++){
                                //printf("debug processUpdates() : k = %d \n", k);
                                for(int l = 0; l < UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[1][2]].NumberOfFreeRowLineElementsToTrigger[k]; l++){
                                    //printf("debug processUpdates() : l = %d \n", l);
                                    int freeRowLineIndex = UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[1][2]].TriggerFreeRowLineIndices[k];
                                    //printf("debug processUpdates() : freeRowLineIndex = %d \n", freeRowLineIndex);
                                    if(UiPanes[i].freeRowLines[freeRowLineIndex].needsRefresh){
                                        if(UiPanes[i].freeRowLines[freeRowLineIndex].VisualElementBuffer){
                                            //printf("UiPanes[i].FreeBoundingBoxes[uiMouse.mouseUi_PreviousLocation_FreeBoundingBoxIndex].TriggerFreeElementIndices[k][l] = %d \n",UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[1][2]].TriggerFreeElementIndices[k][l]);
                                            UiPanes[i].freeRowLines[freeRowLineIndex].VisualElementBuffer[UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[1][2]].TriggerFreeElementIndices[k][l]].render(this,0);

                                        }
                                        else if(UiPanes[i].freeRowLines[freeRowLineIndex].TextBoxBuffer){
                                            //printf("UiPanes[i].FreeBoundingBoxes[uiMouse.mouseUi_PreviousLocation_FreeBoundingBoxIndex].TriggerFreeElementIndices[k][l] = %d \n",UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[1][2]].TriggerFreeElementIndices[k][l]);
                                            UiPanes[i].freeRowLines[freeRowLineIndex].TextBoxBuffer[UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[1][2]].TriggerFreeElementIndices[k][l]].render(this,0);

                                        }
                                        
                                        UiPanes[i].freeRowLines[freeRowLineIndex].needsRefresh = 0 + UiPanes[i].freeRowLines[freeRowLineIndex].needsRefresh * isAnimated;

                                    }
                                    
                                    
                                }

                            }
                        }
                        
                        if(uiMouse.locationData[0][2] >= 0){ //is there a reason we are not checking uiMouse.locationData[0][1] == 3?
                            for(int k = 0; k < UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[0][2]].NumberOfFreeRowLinesToTrigger; k++){
                                printf("debug processUpdates() : k = %d \n", k);
                                for(int l = 0; l < UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[0][2]].NumberOfFreeRowLineElementsToTrigger[k]; l++){
                                    /**/
                                    printf("debug processUpdates() : l = %d \n", l);
                                    int freeRowLineIndex = UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[0][2]].TriggerFreeRowLineIndices[k];
                                    printf("debug processUpdates() : freeRowLineIndex = %d \n", freeRowLineIndex);
                                    if(UiPanes[i].freeRowLines[freeRowLineIndex].needsRefresh){
                                        if(UiPanes[i].freeRowLines[freeRowLineIndex].VisualElementBuffer){
                                            printf("UiPanes[i].FreeBoundingBoxes[uiMouse.mouseUi_CurrentLocation_FreeBoundingBoxIndex].TriggerFreeElementIndices[k][l] = %d \n",UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[0][2]].TriggerFreeElementIndices[k][l]);
                                            UiPanes[i].freeRowLines[freeRowLineIndex].VisualElementBuffer[UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[0][2]].TriggerFreeElementIndices[k][l]].render(this,0);

                                        }
                                        else if(UiPanes[i].freeRowLines[freeRowLineIndex].TextBoxBuffer){
                                            printf("UiPanes[i].FreeBoundingBoxes[uiMouse.mouseUi_CurrentLocation_FreeBoundingBoxIndex].TriggerFreeElementIndices[k][l] = %d \n",UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[0][2]].TriggerFreeElementIndices[k][l]);
                                            UiPanes[i].freeRowLines[freeRowLineIndex].TextBoxBuffer[UiPanes[i].FreeBoundingBoxes[uiMouse.locationData[0][2]].TriggerFreeElementIndices[k][l]].render(this,0);

                                        }
                                        
                                        UiPanes[i].freeRowLines[freeRowLineIndex].needsRefresh = 0 + UiPanes[i].freeRowLines[freeRowLineIndex].needsRefresh * isAnimated;

                                    }
                                    
                                    
                                }

                            }
                        }

                        UiPanes[i].freeRowLinesNeedRefresh = 0 + UiPanes[i].freeRowLinesNeedRefresh * isAnimated;
                    }
                

                }
                



                
                //UiPanes[i].NeedsRefresh = 0 + UiPanes[i].NeedsRefresh * isAnimated;
            }

        }

        mergeSortUpdateBlocks(0,(*UpdateBlockQueueIndex)-1);

        




        if(uiMouse.locationData[0][1] < -1){
            uiMouse.setMouseCursorContext(uiMouse.locationData[0][1]);
        }






        /**/
        updateBlock mouseUpdate = {};
        mouseUpdate.ID = -9999;
        mouseUpdate.width = 14;
        mouseUpdate.height = 20;
        if(uiMouse.locationData[0][1] < -1){
            if(uiMouse.locationData[0][1] == -2 || uiMouse.locationData[0][1] == -3){ //left/right
                mouseUpdate.width = 20;
                mouseUpdate.height = 10;
                mouseUpdate.topLeftPixelCoordinateX = uiMouse.mouseXY.mousePixelCoordinateX - 10;
                mouseUpdate.topLeftPixelCoordinateY = uiMouse.mouseXY.mousePixelCoordinateY - 5;

            }
            else if(uiMouse.locationData[0][1] == -4 || uiMouse.locationData[0][1] == -5){ //top/bottom
                mouseUpdate.width = 10;
                mouseUpdate.height = 20;
                mouseUpdate.topLeftPixelCoordinateX = uiMouse.mouseXY.mousePixelCoordinateX - 5;
                mouseUpdate.topLeftPixelCoordinateY = uiMouse.mouseXY.mousePixelCoordinateY - 10;
                
            }


        }
        else{  
            mouseUpdate.topLeftPixelCoordinateX = uiMouse.mouseXY.mousePixelCoordinateX;//mouseX;
            mouseUpdate.topLeftPixelCoordinateY = uiMouse.mouseXY.mousePixelCoordinateY;//mouseY;
        }
        if((*UpdateBlockQueueIndex) != 0){
            mouseUpdate.Layer = UpdateBlockQueue[(*UpdateBlockQueueIndex)-1].Layer + 1;
        }
        else{
            mouseUpdate.Layer = 0;

        }

        
        
        //uiMouse.render();
        sendUpdateBlock(1,&mouseUpdate);

       
        

        updateNumberOfDispatches();


        //TotalNumberOfPixelsInUpdateBlockQueue = 0;
        memset(TotalNumberOfPixelsInDispatch,0,UpdateBlockQueueBufferSize * sizeof(int));



        //printf(" \n \n \n STARTING DEBUGGING PROCESSUPDATES(): \n");
        if(*UpdateBlockQueueIndex == 0){
            printf("DEBUGGING PROCESSUPDATES(): PROCESSUPDATES() EXITS EARLY BECAUSE UPDATEBLOCKQUEUEINDEX == 0 \n");
            return;
        }
        printf("DEBUGGING PROCESSUPDATES(): UPDATEBLOCKQUEUEINDEX IS: %d \n",*UpdateBlockQueueIndex);
        printf("DEBUGGING PROCESSUPDATES(): ENTERING FOR LOOP \n");
        for(int i = 0,k=0; i < *UpdateBlockQueueIndex; i++){
            printf("DEBUGGING PROCESSUPDATES(): i = %d \n", i);
            //printf("DEBUGGING PROCESSUPDATES(): UpdateBlockQueue[%d].pixelIndex = %d \n", i,TotalNumberOfPixelsInUpdateBlockQueue) ;
            if((k+1 < NumberOfDispatches) && (i == DispatchStartIndices[k+1])){
                k = k +1;
                printf("DEBUGGING PROCESSUPDATES(): if(i == DispatchStartIndices[k+1]){} : k = %d \n", k);

            }
            UpdateBlockQueue[i].pixelIndex = TotalNumberOfPixelsInDispatch[k];
            printf("DEBUGGING PROCESSUPDATES(): UpdateBlockQueue[i].pixelIndex = %d \n", UpdateBlockQueue[i].pixelIndex);
            //printf("DEBUGGING PROCESSUPDATES(): TotalNumberOfPixelsInUpdateBlockQueue = %d + (%d * %d) \n",TotalNumberOfPixelsInUpdateBlockQueue,UpdateBlockQueue[i].width,UpdateBlockQueue[i].height);
            TotalNumberOfPixelsInDispatch[k] = TotalNumberOfPixelsInDispatch[k] + (UpdateBlockQueue[i].width * UpdateBlockQueue[i].height);
            printf("DEBUGGING PROCESSUPDATES(): TotalNumberOfPixelsInDispatch[%d] = %d \n",k,TotalNumberOfPixelsInDispatch[k]);
        }





        printf("EXITING processUpdates(){} \n");


    }


    void processMouseUpdate2(int rawXAxisMovement,int rawYAxisMovement,mouseCoordinates* mouseXY){

        int newMouseX = mouseXY->mousePixelCoordinateX + rawXAxisMovement;//mouseX + rawXAxisMovement;
        if(newMouseX > renderTargetWidth){
            //mouseX = renderTargetWidth;
            mouseXY->mousePixelCoordinateX = renderTargetWidth;
        }
        else if(newMouseX < 0){
            //mouseX = 0;
            mouseXY->mousePixelCoordinateX = 0;
        }
        else{
            //mouseX = newMouseX;
            mouseXY->mousePixelCoordinateX = newMouseX;


        }


        int newMouseY = mouseXY->mousePixelCoordinateY - rawYAxisMovement;//mouseY - rawYAxisMovement;
        if(newMouseY > renderTargetHeight){
            //mouseY = renderTargetHeight;
            mouseXY->mousePixelCoordinateY = renderTargetHeight;
        
        }
        else if(newMouseY < 0){
            //mouseY = 0;
            mouseXY->mousePixelCoordinateY = renderTargetHeight;
        }
        else{
            //mouseY = newMouseY;
            mouseXY->mousePixelCoordinateY = newMouseY;
            

        }
        
    }
    

    bool isMouseInGameViewPane(){
        /*
        if((mouseX < 1640 && mouseX > 56) && (mouseY < 814 && mouseY > 56)){
            return true;
        }
        */
        if((uiMouse.mouseXY.mousePixelCoordinateX < 1640 && uiMouse.mouseXY.mousePixelCoordinateX > 56) && (uiMouse.mouseXY.mousePixelCoordinateY < 814 && uiMouse.mouseXY.mousePixelCoordinateY > 56)){
            return true;
        }
        else{
            return false;
        }

    }

    void deallocateUiPaneBuffer(){
        
    }

   
};



void textBox::render(uiData* ui,rowLineRegion* parentRowLineRegion){
    //UiPanes[uiMouse.mouseUi_CurrentLocation_PaneIndex].RowLineRegion->RowLineBuffer[rowLineBufferIndexStart].TextBoxBuffer[uiMouse.mouseUi_CurrentLocation_UiElementIndex].highlighted = 1;   
    //printf("/t/t textBox::render() : TextBufferIndex = %d, GlyphPixelCoordinatesX[0] = %d, RowLineAlignmentPixelCoordinateY - getGlyphHeight(TextBuffer[0]) = %d \n",TextBufferIndex,GlyphPixelCoordinatesX[0],RowLineAlignmentPixelCoordinateY - getGlyphHeight(TextBuffer[0]));
    
    for(int l = 0; l < TextBufferIndex; l++){//l is each character in textbox
            char character = TextBuffer[l];
            updateBlock characterUpdateBlock = {};
            characterUpdateBlock.ID = character;
            //printf("processUpdates() : \n");
            if(highlighted ==1){
                characterUpdateBlock.ID = characterUpdateBlock.ID + 100;
            }
            //printf("DEBUGGING processUpdates() : characterUpdateBlock.ID = %d \n",characterUpdateBlock.ID);
            characterUpdateBlock.width = getGlyphWidth(character);
            characterUpdateBlock.height = getGlyphHeight(character);
            characterUpdateBlock.topLeftPixelCoordinateX = GlyphPixelCoordinatesX[l];
            characterUpdateBlock.topLeftPixelCoordinateY = RowLineAlignmentPixelCoordinateY - characterUpdateBlock.height;
            characterUpdateBlock.Layer = Layer;
            if(parentRowLineRegion){
                updateBlock overlapRegion = {};
                if(parentRowLineRegion->isUpdateBlockInRowLineRegion(characterUpdateBlock,&overlapRegion)){
                    ui->sendUpdateBlock(1,&overlapRegion);
                }
            }
            else{
                ui->sendUpdateBlock(1,&characterUpdateBlock);

            }

    } 
}


void VisualElement::render(uiData* ui, rowLineRegion* parentRowLineRegion){

    updateBlock visualElementUpdateBlock = {};
    visualElementUpdateBlock.ID = ElementID;
    //printf("processUpdates() : UiPanes[uiMouse.mouseUi_CurrentLocation_PaneIndex].RowLineRegion->RowLineBuffer[rowLineBufferIndexStart+i].uiElementBuffer[uiMouse.mouseUi_CurrentLocation_UiElementIndex].highlighted = %d \n",UiPanes[uiMouse.mouseUi_CurrentLocation_PaneIndex].RowLineRegion->RowLineBuffer[rowLineBufferIndexStart+i].uiElementBuffer[uiMouse.mouseUi_CurrentLocation_UiElementIndex].highlighted);
    if(highlighted > 0){
        visualElementUpdateBlock.ID = visualElementUpdateBlock.ID + 1000 * highlighted;
    }
    visualElementUpdateBlock.width = Width;
    visualElementUpdateBlock.height = Height;
    visualElementUpdateBlock.topLeftPixelCoordinateX = RowLineAlignmentPixelCoordinateX;
    visualElementUpdateBlock.topLeftPixelCoordinateY = RowLineAlignmentPixelCoordinateY - visualElementUpdateBlock.height;
    visualElementUpdateBlock.Layer = Layer;

    if(parentRowLineRegion){
        updateBlock overlapRegion = {};
        if(parentRowLineRegion->isUpdateBlockInRowLineRegion(visualElementUpdateBlock,&overlapRegion)){
            ui->sendUpdateBlock(1,&overlapRegion);
        }
    }
    else{
        ui->sendUpdateBlock(1,&visualElementUpdateBlock);

    }

}

void rowLineAlignment::render(uiData* ui, rowLineRegion* parentRowLineRegion){
        //printf("\t rowLineAlignment::render() : \n");
        if(TextBoxBuffer){
            //printf("\t rowLineAlignment::render() : TextBoxBuffer : TextBoxBufferIndex = %d \n",TextBoxBufferIndex);
            for(int k = 0; k < TextBoxBufferIndex; k++){ //k is each textbox
                TextBoxBuffer[k].render(ui,parentRowLineRegion);
            }

        }
        else if(VisualElementBuffer){
            for(int k = 0; k < VisualElementBufferIndex; k++){ //k is each uiElement
                VisualElementBuffer[k].render(ui,parentRowLineRegion);
            }
        }
        
    }














void rowLineCluster::initializeRowLineCluster(int type,int clusterVerticalOffset,int clusterIndex,int maxNumberOfElements,rowLineRegion* parentRowLineRegion,int startingRowLineIndex,int numberOfRowLines,rowLineRegionElementInfo* elementPrototype,int spacing){
        printf("rowLineCluster::initializeRowLineCluster() : \n ");
         
        //Type = type;
        //TextBoxLength = textBoxLength; //DEPRECATED
        //TextBoxMaxWidth = textBoxMaxWidth; //DEPRECATED

        ElementWidth = 0;
        ElementHeight = 0;
        printf("rowLineCluster::initializeRowLineCluster() : ElementWidth = %d \n ",ElementWidth);

        ParentRowLineRegion = parentRowLineRegion;
        StartingRowLineIndex = startingRowLineIndex;
        RowLineBuffer = ParentRowLineRegion->RowLineBuffer;

        ClusterIndex = clusterIndex;

        NumberOfRowLinesInCluster = numberOfRowLines;
        NumberOfElementsInCluster = 0;
        NumberOfRowLinesPerElement = (int*)malloc(maxNumberOfElements * sizeof(int)); 
        ElementStartingX = (int*)malloc(maxNumberOfElements * sizeof(int));

        ElementPrototype = elementPrototype;

        ClusterVerticalOffset = clusterVerticalOffset;

        ElementTopSideYValue = ElementPrototype->firstElementPixelCoordinateY;
        ElementBottomSideYValue = 0;


        HorizontalSpacing = spacing;
        ElementStartingX[0] = ElementPrototype->firstElementPixelCoordinateX; //+ parentRowLineRegion->LeftSideXValue;

        for(int i = 0; i < NumberOfRowLinesInCluster; i++){
            if(ElementPrototype->ordering[i] == 0){
                if((ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateX + ElementPrototype->textBoxes[i].Width) > ElementWidth){
                    ElementWidth = (ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateX + ElementPrototype->textBoxes[i].Width);
                }
           
                if((ElementPrototype->firstElementPixelCoordinateY + ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateY - ElementPrototype->textBoxes[i].Height) < ElementTopSideYValue){
                    ElementTopSideYValue = ElementPrototype->firstElementPixelCoordinateY + ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateY + ElementPrototype->textBoxes[i].Height;
                }
                if((ElementPrototype->firstElementPixelCoordinateY + ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateY) > ElementBottomSideYValue){
                    ElementBottomSideYValue = ElementPrototype->firstElementPixelCoordinateY + ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateY;
                }
                
                createRowLine(i,0,maxNumberOfElements,ElementPrototype->textBoxes[i].Layer,ClusterVerticalOffset + ElementPrototype->firstElementPixelCoordinateY+ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateY);
            }

            else if (ElementPrototype->ordering[i] == 1){
                if((ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateX + ElementPrototype->VisualElements[i].Width) > ElementWidth){
                    ElementWidth = (ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateX + ElementPrototype->VisualElements[i].Width);
                }
                if((ElementPrototype->firstElementPixelCoordinateY + ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateY - ElementPrototype->VisualElements[i].Height) < ElementTopSideYValue){
                    ElementTopSideYValue = (ElementPrototype->firstElementPixelCoordinateY + ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateY - ElementPrototype->VisualElements[i].Height);
                }
                if((ElementPrototype->firstElementPixelCoordinateY + ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateY) > ElementBottomSideYValue){
                    ElementBottomSideYValue = ElementPrototype->firstElementPixelCoordinateY + ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateY;
                }
                

                createRowLine(i,1,maxNumberOfElements,ElementPrototype->VisualElements[i].Layer,ClusterVerticalOffset + ElementPrototype->firstElementPixelCoordinateY+ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateY);

            }
            else{
                //printf("ROWLINEREGION SHOULD NOT PRINT THIS \n");
            }
        }

        
        //EDIT 05/03/2025 since ElementPrototype->firstElementPixelCoordinateX will now be changed to offset of rowlineregions bounds we add the leftSideValue of rowlineregion
        StartingElementPositionX = ElementPrototype->firstElementPixelCoordinateX; // + parentRowLineRegion->LeftSideXValue
        ElementHeight = ElementBottomSideYValue - ElementTopSideYValue; //Remember the Ui System folls the same coordinate system as glsl, so that means x increases from left to right, y increases from top to bottom, therefore height (which is a positive value) needs to be bottomCoordinate-topCoordinate

        printf("rowLineCluster::initializeRowLineCluster() : ElementHeight = %d \n ",ElementHeight);
        printf("rowLineCluster::initializeRowLineCluster() : ElementWidth = %d \n ",ElementWidth); 
    }



    //TODO

    void rowLineCluster::initializeRowLineCluster2(int clusterVerticalOffset,int clusterIndex,int maxNumberOfElements,rowLineRegion* parentRowLineRegion,int startingRowLineIndex,int numberOfRowLines,rowLineRegionElementInfo* elementPrototype,int elementListStartIndex, int elementListEndIndex, bool isSpecialCluster , int NumberOfInsertionPoints,int* InsertionPoints, int* NumberOfExtraRowlinesNeededAtInsertionPoint){
        

        ElementWidth = 0;
        ElementHeight = 0;
        

        ParentRowLineRegion = parentRowLineRegion;
        StartingRowLineIndex = startingRowLineIndex;
        RowLineBuffer = ParentRowLineRegion->RowLineBuffer;

        ClusterIndex = clusterIndex;

        NumberOfRowLinesInCluster = numberOfRowLines; //+ totalExtraNumberOfRowlines;
        //then intialize Ordering with malloc memory
        Ordering = (bool*)malloc(sizeof(bool) * NumberOfRowLinesInCluster);


        NumberOfElementsInCluster = 0;
        NumberOfRowLinesPerElement = (int*)malloc(maxNumberOfElements * sizeof(int));   
        ElementStartingX = (int*)malloc(maxNumberOfElements * sizeof(int));

        ElementPrototype = elementPrototype;

        ClusterVerticalOffset = clusterVerticalOffset;

        ElementTopSideYValue = ElementPrototype->firstElementPixelCoordinateY;
        ElementBottomSideYValue = 0;


        int currentYOffsetToAddFromExtraRowLines = 0;
        
        ElementStartingX[0] = ElementPrototype->firstElementPixelCoordinateX; + parentRowLineRegion->LeftSideXValue;

        int numberOfInsertedRowLinesEncountered = 0;
        for(int i =0; i < NumberOfRowLinesInCluster; i++){

            if(Ordering[i+numberOfInsertedRowLinesEncountered] == 0){
                if(isSpecialCluster==1 && InsertionPoints[numberOfInsertedRowLinesEncountered] == i){
                    for(int j = 0; j < NumberOfExtraRowlinesNeededAtInsertionPoint[numberOfInsertedRowLinesEncountered]; j++){
                        Ordering[i + numberOfInsertedRowLinesEncountered] = -1; //-1 INDICATES this rowline is TEXTBOX_WRAPAROUND, whereas 0 is TEXTBOX
                        numberOfInsertedRowLinesEncountered = numberOfInsertedRowLinesEncountered + 1;
                    }
                }

            }
            if(Ordering[i+numberOfInsertedRowLinesEncountered] == 1){

            }
            

        }
        


        for(int i = 0; i < ElementPrototype->numberOfRowLinesPerElement; i++){

            
            if(ElementPrototype->ordering[i] == 0){

                for(int j = 0; j < 1 + (((isSpecialCluster==1) && (InsertionPoints[i]==i)) * NumberOfExtraRowlinesNeededAtInsertionPoint[i]); j++){
                    if(j > 0){currentYOffsetToAddFromExtraRowLines = currentYOffsetToAddFromExtraRowLines + ElementPrototype->textBoxes[i].Height + ElementPrototype->textBoxWrapAroundSpacing[i];}
                    if((ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateX + ElementPrototype->textBoxes[i].Width) > ElementWidth){
                        ElementWidth = (ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateX + ElementPrototype->textBoxes[i].Width);
                    }
            
                    if((ElementPrototype->firstElementPixelCoordinateY + currentYOffsetToAddFromExtraRowLines + ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateY - ElementPrototype->textBoxes[i].Height) < ElementTopSideYValue){
                        ElementTopSideYValue = ElementPrototype->firstElementPixelCoordinateY + currentYOffsetToAddFromExtraRowLines + ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateY + ElementPrototype->textBoxes[i].Height;
                    }
                    if((ElementPrototype->firstElementPixelCoordinateY + currentYOffsetToAddFromExtraRowLines + ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateY) > ElementBottomSideYValue){
                        ElementBottomSideYValue = ElementPrototype->firstElementPixelCoordinateY + currentYOffsetToAddFromExtraRowLines + ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateY;
                    }
                    
                    createRowLine(i,0,maxNumberOfElements,ElementPrototype->textBoxes[i].Layer,ClusterVerticalOffset + ElementPrototype->firstElementPixelCoordinateY + ElementPrototype->textBoxes[i].RowLineAlignmentPixelCoordinateY);
                }
            }

            else if (ElementPrototype->ordering[i] == 1){
                if((ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateX + ElementPrototype->VisualElements[i].Width) > ElementWidth){
                    ElementWidth = (ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateX + ElementPrototype->VisualElements[i].Width);
                }
                if((ElementPrototype->firstElementPixelCoordinateY + currentYOffsetToAddFromExtraRowLines + ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateY - ElementPrototype->VisualElements[i].Height) < ElementTopSideYValue){
                    ElementTopSideYValue = (ElementPrototype->firstElementPixelCoordinateY + currentYOffsetToAddFromExtraRowLines+ ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateY - ElementPrototype->VisualElements[i].Height);
                }
                if((ElementPrototype->firstElementPixelCoordinateY + currentYOffsetToAddFromExtraRowLines + ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateY) > ElementBottomSideYValue){
                    ElementBottomSideYValue = ElementPrototype->firstElementPixelCoordinateY + currentYOffsetToAddFromExtraRowLines + ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateY;
                }
                

                createRowLine(i,1,maxNumberOfElements,ElementPrototype->VisualElements[i].Layer,ClusterVerticalOffset + ElementPrototype->firstElementPixelCoordinateY+ElementPrototype->VisualElements[i].RowLineAlignmentPixelCoordinateY);

            }
            else{
                //printf("SHOULD NOT PRINT THIS \n");
            }
        }

        

        
        //EDIT 05/03/2025 since ElementPrototype->firstElementPixelCoordinateX will now be changed to offset of rowlineregions bounds we add the leftSideValue of rowlineregion
        StartingElementPositionX = ElementPrototype->firstElementPixelCoordinateX + parentRowLineRegion->LeftSideXValue;
        ElementHeight = ElementBottomSideYValue - ElementTopSideYValue; //Remember the Ui System folls the same coordinate system as glsl, so that means x increases from left to right, y increases from top to bottom, therefore height (which is a positive value) needs to be bottomCoordinate-topCoordinate

        printf("rowLineCluster::initializeRowLineCluster() : ElementHeight = %d \n ",ElementHeight);
        printf("rowLineCluster::initializeRowLineCluster() : ElementWidth = %d \n ",ElementWidth); 
    }


    int rowLineCluster::centerElement(int elementIndex){
        for(int i = 0, j = 0; i < NumberOfRowLinesPerElement[elementIndex]; i++){
            printf("rowLineCluster::centerElement() : for{} : j = %d \n",j);
            if(ElementPrototype->ordering[j] == 0){
                int centerOnWidth = 0; //this is the width of the visualElement/textbox we are centering
                int centerOnShiftAmount = 0; //this is the amount of space the visualElement/textbox we are centering on has been shifted from the uiElements leftmostX bound (likely shifted because it is in turn centered)
                if(ElementPrototype->centerOn_index[j] == 9999){
                    centerOnWidth = ElementWidth;
                }
                if(ElementPrototype->ordering[ElementPrototype->centerOn_index[j]] == 0){
                    centerOnWidth = RowLineBuffer[StartingRowLineIndex+ElementPrototype->centerOn_index[j]].TextBoxBuffer[elementIndex].Width;
                    centerOnShiftAmount = RowLineBuffer[StartingRowLineIndex+ElementPrototype->centerOn_index[j]].TextBoxBuffer[elementIndex].RowLineAlignmentPixelCoordinateX - ElementStartingX[elementIndex];
                }
                else if(ElementPrototype->ordering[ElementPrototype->centerOn_index[j]] == 1){
                    centerOnWidth = RowLineBuffer[StartingRowLineIndex+ElementPrototype->centerOn_index[j]].VisualElementBuffer[elementIndex].Width;
                    centerOnShiftAmount = RowLineBuffer[StartingRowLineIndex+ElementPrototype->centerOn_index[j]].VisualElementBuffer[elementIndex].RowLineAlignmentPixelCoordinateX - ElementStartingX[elementIndex];
                }
                int shiftAmount = (centerOnWidth - RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer[RowLineBuffer[StartingRowLineIndex+i].TextBoxBufferIndex-1].Width)/2;
                shiftAmount = shiftAmount + centerOnShiftAmount;
                RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer[RowLineBuffer[StartingRowLineIndex+i].TextBoxBufferIndex-1].horizontalShift(shiftAmount);
                        

            }

            if(ElementPrototype->ordering[j] == 1){
                int centerOnWidth = 0; //this is the width of the visualElement/textbox we are centering
                int centerOnShiftAmount = 0; //this is the amount of space the visualElement/textbox we are centering on has been shifted from the uiElements leftmostX bound (likely shifted because it is in turn centered)
                if(ElementPrototype->centerOn_index[j] == 9999){
                    centerOnWidth = ElementWidth;
                }
                int shiftAmount = (centerOnWidth - RowLineBuffer[StartingRowLineIndex+i].VisualElementBuffer[RowLineBuffer[StartingRowLineIndex+i].VisualElementBufferIndex-1].Width)/2;
                RowLineBuffer[StartingRowLineIndex+i].VisualElementBuffer[RowLineBuffer[StartingRowLineIndex+i].VisualElementBufferIndex-1].horizontalShift(shiftAmount);

            }


            if(i < ElementPrototype->numberOfRowLinesPerElement - 1){
                j = j + 1;
            }
        }
        return 0;

    }

    //int numberOfExtraRowLinesInElement = 0;
    //for(int i = 0,j=0,k=0; i < ElementPrototype->numberOfRowLinesPerElement; i++){ //i is the rowLine


    //What is addElement2() resposible for? Or more correctly what does the refactor need it for, since extra rowline is no longer needed
    //I also wonder
    /*
        -
        -
        -
        -
    */

    int rowLineCluster::addElement2(int* uiElementIDs,char** textBoxStrings){
        
        int numberOfExtraRowLinesInElement = 0;
        

        for(int i = 0,j=0,k=0; i < ElementPrototype->numberOfRowLinesPerElement; i++){ //i is the rowLine
            
            if(ElementPrototype->ordering[i] == 0){
                int TextBoxLength = ElementPrototype->textBoxMaxStringLength[i];
                int TextBoxMaxWidth = ElementPrototype->textBoxMaxWidth[i];

                
                int xOffset = ElementStartingX[NumberOfElementsInCluster];
                int stringSize = getStringLength(textBoxStrings[j]);
                if((ElementPrototype->textBoxWrapAround[i] == 1 && i == ElementPrototype->numberOfRowLinesPerElement-1 )){
                    
                    int numberOfCharactersLeft = stringSize;
                    while(numberOfCharactersLeft > 0){
                        
                        if(i > NumberOfRowLinesInCluster-1){
                            
                            appendRowLineToCluster(ParentRowLineRegion); //remember that this will increment the NumberofRowLinesInCluster by one
                            numberOfExtraRowLinesInElement = numberOfExtraRowLinesInElement + 1;
                        }
                        
                        numberOfCharactersLeft = RowLineBuffer[StartingRowLineIndex+i].createTextBox(TextBoxLength,xOffset,12,textBoxStrings[j],numberOfCharactersLeft,TextBoxMaxWidth);
                        i = i + 1;

                    }
                       
                    break;
                }
                else{
                    RowLineBuffer[StartingRowLineIndex+i].createTextBox(TextBoxLength,xOffset,12,textBoxStrings[j],stringSize,TextBoxMaxWidth);
                    RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer[RowLineBuffer[StartingRowLineIndex+i].TextBoxBufferIndex-1].horizontalShift((ElementWidth - RowLineBuffer[StartingRowLineIndex+i].TextBoxBuffer[RowLineBuffer[StartingRowLineIndex+i].TextBoxBufferIndex-1].Width)/2);
                }
                
                
                
                j = j + 1;

            }
            else if(ElementPrototype->ordering[i] == 1){

                int xOffset = ElementStartingX[NumberOfElementsInCluster];
                RowLineBuffer[StartingRowLineIndex+i].createVisualElement(xOffset,uiElementIDs[k],ElementPrototype->VisualElements[i].Width,ElementPrototype->VisualElements[i].Height);
                int shiftAmount = 0;
 
                
                k = k + 1;
                

            }
            else{
                //printf("ROWLINEREGION : addElement SHOULD NOT PRINT THIS \n");

            }

        }

        
        NumberOfRowLinesPerElement[NumberOfElementsInCluster] = ElementPrototype->numberOfRowLinesPerElement + numberOfExtraRowLinesInElement;
        
        centerElement(NumberOfElementsInCluster);


        NumberOfElementsInCluster = NumberOfElementsInCluster + 1;

        

        ElementStartingX[NumberOfElementsInCluster] = ElementStartingX[NumberOfElementsInCluster-1] + ElementWidth + HorizontalSpacing;
        

        
        if(((ElementStartingX[NumberOfElementsInCluster] + ElementWidth) > ParentRowLineRegion->RightSideXValue)){
            
            return 1;


        }
        return 0;
        


    }

    int rowLineCluster::addElement3(int* uiElementIDs,char** textBoxStrings){
        
        //int numberOfExtraRowLines = NumberOfExtraRowLinesInCluster; TODO FIX
        int numberOfCharactersLeft = -1;

        for(int i = 0,j=0,k=0; i < NumberOfRowLinesInCluster; i++){ //i is the rowLine
            int xOffset = ElementStartingX[NumberOfElementsInCluster];
            if(Ordering[i] == 0){
                int TextBoxLength = ElementPrototype->textBoxMaxStringLength[i];
                

                
                
                int stringSize = getStringLength(textBoxStrings[j]);

                
            
                numberOfCharactersLeft = RowLineBuffer[StartingRowLineIndex+i].createTextBox(TextBoxLength,xOffset,12,textBoxStrings[j],stringSize,ElementPrototype->textBoxMaxWidth[i]);
                
                
                
                
                j = j + 1;

            }
            //
            else if(Ordering[i] == 1){

                
                RowLineBuffer[StartingRowLineIndex+i].createVisualElement(xOffset,uiElementIDs[k],ElementPrototype->VisualElements[i].Width,ElementPrototype->VisualElements[i].Height);
                int shiftAmount = 0;
 
                
                k = k + 1;
                

            }


            
            //TextBoxWrapAround Case (negative numbers indicate an extra rowline case)
            if(Ordering[i] == -1){
                int TextBoxLength = ElementPrototype->textBoxMaxStringLength[i];
                int stringSize = getStringLength(textBoxStrings[j]);
                if(numberOfCharactersLeft <= 0){return -1;} //ERROR
                numberOfCharactersLeft = RowLineBuffer[StartingRowLineIndex+i].createTextBox(TextBoxLength,xOffset,12,textBoxStrings[j]+(stringSize-numberOfCharactersLeft),numberOfCharactersLeft,ElementPrototype->textBoxMaxWidth[i]); //is 4th parameter off by one?
 
            }
            else{
                //printf("ROWLINEREGION : addElement SHOULD NOT PRINT THIS \n");

            }

        }

        //In the line below "numberOfExtraRowlines" is logically the 'number of extra rowlines unused' which is why we subtract it from "NumberOfExtraRowLinesInCluster" to get the number of rowlines in this element

        //NumberOfRowLinesPerElement[NumberOfElementsInCluster] = ElementPrototype->numberOfRowLinesPerElement + NumberOfExtraRowLinesInCluster - numberOfExtraRowLines; TODO FIX
        
        centerElement(NumberOfElementsInCluster);


        NumberOfElementsInCluster = NumberOfElementsInCluster + 1;

        

        ElementStartingX[NumberOfElementsInCluster] = ElementStartingX[NumberOfElementsInCluster-1] + ElementWidth + HorizontalSpacing;
        

        //i just realized shouldnt this also include horizontal spacing??
        if(((ElementStartingX[NumberOfElementsInCluster] + ElementWidth) > ParentRowLineRegion->RightSideXValue)){
            
            return 1;


        }
        return 0;
        


    }


    void rowLineCluster::createRowLine(int rowLineBufferIndex, bool textBoxOrUiElement,int bufferSize, int layer, int pixelCoordinateY){
       RowLineBuffer[StartingRowLineIndex+rowLineBufferIndex] = rowLineAlignment(textBoxOrUiElement,bufferSize,layer,pixelCoordinateY);


    }

    int rowLineCluster::findMouseLocation(mouse* uiMouse){
        printf("rowLineCluster :: findMouseLocation() : ClusterIndex = %d \n",ClusterIndex);
        int mouseX = uiMouse->mouseXY.mousePixelCoordinateX;
        int mouseY = uiMouse->mouseXY.mousePixelCoordinateY;
        
        uiMouse->locationData[0][2] = -1;
        uiMouse->locationData[0][3] = -1;

        int topLeftX = 0;
        int topLeftY = RowLineBuffer[StartingRowLineIndex].PixelCoordinateY - RowLineBuffer[StartingRowLineIndex].RowLineBoundingHeight;
        int bottomRightX = 9999;
        int bottomRightY = RowLineBuffer[StartingRowLineIndex + NumberOfRowLinesInCluster - 1].PixelCoordinateY;
        printf("rowLineCluster :: findMouseLocation() : topLeftX = %d  topLeftY = %d bottomRightX = %d bottomRightY = %d \n",topLeftX, topLeftY, bottomRightX, bottomRightY);

        if(isPointInRectangle(mouseX,mouseY,topLeftX,topLeftY,bottomRightX,bottomRightY)){
            uiMouse->locationData[0][2] = ClusterIndex;
            uiMouse->locationDepth[0] = 2;

            //check if mouse is in the i'th visible rowLineCluster
               
            int elementHorizontalIndex = 0;
            int nextElementPositionX = StartingElementPositionX;
            
            //printf("nextElementPositionX = %d, ElementWidth = %d, HorizontalSpacing = %d, ParentRowLineRegion->RightSideXValue = %d, elementHorizontalIndex = %d, NumberOfElementsInCluster = %d \n",
                   // nextElementPositionX,ElementWidth,HorizontalSpacing,ParentRowLineRegion->RightSideXValue,elementHorizontalIndex,NumberOfElementsInCluster);

            //find which uiElement in the i'th rowLineCluster the mouse is in
            while(((nextElementPositionX + ElementWidth + HorizontalSpacing) <= ParentRowLineRegion->RightSideXValue) && (elementHorizontalIndex <= (NumberOfElementsInCluster - 1))){
                //printf("rowLineCluster :: findMouseLocation() : whileLoop{} :  \n");
                
                //printf("rowLineCluster :: findMouseLocation() : whileLoop{} : nextElementPositionX = %d, RowLineBuffer[StartingRowLineIndex].PixelCoordinateY = %d, RowLineBuffer[StartingRowLineIndex].RowLineBoundingHeight = %d, nextElementPositionX + ElementWidth = %d, RowLineBuffer[StartingRowLineIndex + NumberOfRowLinesInCluster-1].PixelCoordinateY = %d \n",
                //                                                            nextElementPositionX,RowLineBuffer[StartingRowLineIndex].PixelCoordinateY,RowLineBuffer[StartingRowLineIndex].RowLineBoundingHeight,nextElementPositionX + ElementWidth,RowLineBuffer[StartingRowLineIndex + NumberOfRowLinesInCluster-1].PixelCoordinateY);
                if(isPointInRectangle(mouseX, mouseY, nextElementPositionX,RowLineBuffer[StartingRowLineIndex].PixelCoordinateY - RowLineBuffer[StartingRowLineIndex].RowLineBoundingHeight, nextElementPositionX + ElementWidth, RowLineBuffer[StartingRowLineIndex + NumberOfRowLinesInCluster-1].PixelCoordinateY)){
                    //*returnElementIndex = elementHorizontalIndex;
                    //printf("rowLineCluster :: findMouseLocation() : whileLoop{} :  \n");
                    uiMouse->locationData[0][3] = elementHorizontalIndex;
                    uiMouse->locationDepth[0] = 3;
                    return 2;
                }
                
                elementHorizontalIndex = elementHorizontalIndex + 1;
                nextElementPositionX = nextElementPositionX + ElementWidth + HorizontalSpacing;
            }
            
            //if uiMouse.mouseUi_CurrentLocation_UiElementIndex remains -1
            //Then the mouse was in a row line cluster region, but not in the bounds of any one individual ui element (this happens when mouse cursor falls in spacing between elements)

                




            return 1;

        }
        else{
            return 0;
        }

    }






void rowLineCluster::appendRowLineToCluster(rowLineRegion* parentRowLineregion){
    parentRowLineregion->appendRowLineToCluster(ClusterIndex);
    NumberOfRowLinesInCluster = NumberOfRowLinesInCluster + 1;
    printf("void rowLineCluster::appendRowLineToCluster() : ClusterVerticalOffset = %d \n",ClusterVerticalOffset);
    createRowLine(NumberOfRowLinesInCluster - 1,0,parentRowLineregion->MaxNumberOfElementsPerCluster,ElementPrototype->textBoxes[ElementPrototype->numberOfRowLinesPerElement-1].Layer, ElementPrototype->firstElementPixelCoordinateY+ ClusterVerticalOffset + ElementPrototype->textBoxes[ElementPrototype->numberOfRowLinesPerElement-1].RowLineAlignmentPixelCoordinateY + ElementPrototype->textBoxes[ElementPrototype->numberOfRowLinesPerElement-1].Height + ElementPrototype->verticalSpacing );
    ElementHeight = ElementHeight + ElementPrototype->textBoxes[ElementPrototype->numberOfRowLinesPerElement-1].Height + ElementPrototype->verticalSpacing;

}















void rowLineRegion::appendRowLineToCluster(int clusterIndex){
    int startOfShiftIndex = RowLineClusters[clusterIndex].StartingRowLineIndex + RowLineClusters[clusterIndex].NumberOfRowLinesInCluster;
    int shiftAmount = RowLineBufferIndex - startOfShiftIndex;
    printf("rowLineRegion::appendRowLineToCluster() : startOfShiftIndex = %d : shiftAmount = %d : \n", startOfShiftIndex, shiftAmount);
    if(shiftAmount > 0){
        memmove(RowLineBuffer + startOfShiftIndex+1,RowLineBuffer + startOfShiftIndex,sizeof(rowLineAlignment) * shiftAmount);
    }

    RowLineBufferIndex = RowLineBufferIndex + 1;
    RowLineClusterIndices[clusterIndex+1]=  RowLineClusterIndices[clusterIndex+1] + 1;
    

    for(int i = clusterIndex + 1; i < RowLineClusterIndex; i++){
        printf("rowLineRegion::appendRowLineToCluster() : i = %d : \n", i);
        RowLineClusters[i].StartingRowLineIndex = RowLineClusters[i].StartingRowLineIndex + 1;
        int verticalOffset = RowLineClusters[clusterIndex].ElementPrototype->textBoxes[RowLineClusters[clusterIndex].ElementPrototype->numberOfRowLinesPerElement-1].RowLineAlignmentPixelCoordinateY + 20;
        RowLineClusters[i].shiftRowLines(verticalOffset);
    }

}

void rowLineRegion::render(uiData* ui){
    //printf("rowLineRegion::render() : FirstVisibleRowLineIndexTop = %d, LastVisibleRowLineIndexBottom = %d \n",FirstVisibleRowLineIndexTop,LastVisibleRowLineIndexBottom);
    for(int j = FirstVisibleRowLineIndexTop; j <= LastVisibleRowLineIndexBottom; j++){ // j is each is rowLine
        RowLineBuffer[j].render(ui,this);
    }
} 

void rowLineRegion::renderSpecificElement(uiData* ui, int rowLineClusterIndex, int elementIndex){
        int rowLineBufferIndexStart = RowLineClusterIndices[rowLineClusterIndex];
        printf("processUpdates() : rowLineRegion::renderSpecificElement() : rowLineBufferIndexStart = %d \n",rowLineBufferIndexStart);
        printf("processUpdates() : rowLineRegion::renderSpecificElement() : RowLineClusters[rowLineClusterIndex].NumberOfRowLinesPerElement[elementIndex] = %d \n",RowLineClusters[rowLineClusterIndex].NumberOfRowLinesPerElement[elementIndex]);

        for(int i = 0; i < RowLineClusters[rowLineClusterIndex].NumberOfRowLinesPerElement[elementIndex]; i++){
            printf("processUpdates() : rowLineRegion::renderSpecificElement() : for i = %d : \n",i);
        
            
            if(RowLineBuffer[rowLineBufferIndexStart+i].VisualElementBuffer){
                RowLineBuffer[rowLineBufferIndexStart+i].VisualElementBuffer[elementIndex].render(ui,this);

            }
            else if(RowLineBuffer[rowLineBufferIndexStart+i].TextBoxBuffer){     
                if(i >= RowLineClusters[rowLineClusterIndex].ElementPrototype->numberOfRowLinesPerElement){
                    for(int j = 0; j < RowLineBuffer[rowLineBufferIndexStart+i].TextBoxBufferIndex; j++){
                        if((RowLineBuffer[rowLineBufferIndexStart+i].TextBoxBuffer[j].GlyphPixelCoordinatesX[0] >= RowLineClusters[elementIndex].ElementWidth * j) && (RowLineBuffer[rowLineBufferIndexStart+i].TextBoxBuffer[j].GlyphPixelCoordinatesX[0] < RowLineClusters[elementIndex].ElementWidth * (j+1))){
                            elementIndex = j;
                        }
                    }
                    elementIndex = 0;
                }
                RowLineBuffer[rowLineBufferIndexStart+i].TextBoxBuffer[elementIndex].render(ui,this);
            }
        }

    }


void uiPane::renderScrollBars(uiData* ui, int refreshPaneID){
    printf("processUpdates() : Scrollbars :: \n");
        //render scrollBars first
        if(Scrollbars){

        
            if(Scrollbars[1].Type == 1){

            }
            printf("processUpdates() : Scrollbars :: :: \n");
            if(Scrollbars[0].Type == 0){
                printf("processUpdates() : Scrollbars[0].needsRefresh = %d \n",Scrollbars[0].needsRefresh);
                if(Scrollbars[0].needsRefresh || ui->isAnimated){
                    updateBlock scrollRegionUpdateBlock = {};
                    scrollRegionUpdateBlock.ID = Scrollbars[0].ID; //200,001
                    scrollRegionUpdateBlock.topLeftPixelCoordinateX = Scrollbars[0].ScrollableRegionTopLeftPixelX;
                    scrollRegionUpdateBlock.topLeftPixelCoordinateY = Scrollbars[0].ScrollableRegionTopLeftPixelY;
                    scrollRegionUpdateBlock.width = Scrollbars[0].ScrollableRegionWidth;
                    scrollRegionUpdateBlock.height = Scrollbars[0].ScrollableRegionHeight;
                    scrollRegionUpdateBlock.Layer = Scrollbars[0].Layer + 1;

                    updateBlock scrollRegionSliderUpdateBlock = {};
                    scrollRegionSliderUpdateBlock.ID = Scrollbars[0].ID+1; //200,002
                    scrollRegionSliderUpdateBlock.topLeftPixelCoordinateX = Scrollbars[0].ScrollableRegionTopLeftPixelX;
                    scrollRegionSliderUpdateBlock.topLeftPixelCoordinateY = Scrollbars[0].ScrollBarStartOfSliderPosition;
                    scrollRegionSliderUpdateBlock.width = Scrollbars[0].ScrollableRegionWidth;
                    scrollRegionSliderUpdateBlock.height = Scrollbars[0].ScrollBarSliderSize;
                    scrollRegionSliderUpdateBlock.Layer = Scrollbars[0].Layer + 2;

                    if(Scrollbars[0].highLighted > 0){
                        scrollRegionUpdateBlock.ID = scrollRegionUpdateBlock.ID + 100 * Scrollbars[0].highLighted;
                        scrollRegionSliderUpdateBlock.ID = scrollRegionSliderUpdateBlock.ID + 100 * Scrollbars[0].highLighted;
                    }
                    
                    
                    if(ui->UiPanes[refreshPaneID].NeedsRefresh[2] == 1){
                        updateBlock overlapRegionWithPane = {};
                        ui->isUpdateBlockInPane(scrollRegionUpdateBlock,refreshPaneID,&overlapRegionWithPane);
                        ui->sendUpdateBlock(1,&overlapRegionWithPane);
                    }
                    
                    

                    ui->sendUpdateBlock(1,&scrollRegionUpdateBlock);
                    ui->sendUpdateBlock(1,&scrollRegionSliderUpdateBlock);
                    Scrollbars[0].needsRefresh = 0;
                }

        }
    }
}






struct uiToVulkanData{
    int numberOfUpdateBlocks;
    float pad[3];
    updateBlock updateQueue[];
};

struct uiToVulkanPushConstantData{
    int testValue;
    int updateBlockFirstIndex;
    int updateBlockLastIndex;
    float animTiming;
    float animTiming2;
    int currentFrame;
};





