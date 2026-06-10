/*
 * Copyright (c) 2026 Logan Mottern
 * All rights reserved.
 *
 * This source code is provided for portfolio and review purposes only.
 * No permission is granted to copy, modify, distribute, sublicense, or use
 * this software except with the express written permission of the copyright owner.
 */

#pragma once
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include "uiStructs5.h"
void merge(int* array,int first, int middle, int last){
    int leftSize = middle - first + 1;
    int rightSize = last - middle;

    int* left = (int*)malloc(sizeof(int) * (leftSize + 1));
    int* right = (int*)malloc(sizeof(int) * (rightSize + 1));

    for(int i = 0; i < leftSize; i++){
        left[i] = array[first+i];
    }
    for(int i = 0; i < rightSize; i++){
        right[i] = array[middle + i + 1];
    }

    int sentinel = 2147483647;
    left[leftSize] = sentinel;
    right[rightSize] = sentinel;

    for(int i = 0, j =0, k = first; k <= last; k++){
        if(left[i] <= right[j]){
            array[k] = left[i];
            i++;
        }
        else{
            array[k] = right[j];
            j++;
        }



    }
    free(left);
    free(right);
}

void merge_Pane(uiPane* uiPanes,int first, int middle, int last){
    int leftSize = middle - first + 1;
    int rightSize = last - middle;

    uiPane* left = (uiPane*)malloc(sizeof(uiPane) * (leftSize + 1));
    uiPane* right = (uiPane*)malloc(sizeof(uiPane) * (rightSize + 1));

    for(int i = 0; i < leftSize; i++){
        left[i] = uiPanes[first+i];
    }
    for(int i = 0; i < rightSize; i++){
        right[i] = uiPanes[middle + i + 1];
    }

    uiPane sentinelPane = uiPane(1);

    left[leftSize] = sentinelPane;
    right[rightSize] = sentinelPane;

    for(int i = 0, j =0, k = first; k <= last; k++){
        if(left[i].Layer <= right[j].Layer){
            uiPanes[k] = left[i];
            i++;
        }
        else{
            uiPanes[k] = right[j];
            j++;
        }



    }
    free(left);
    free(right);
}

void merge_rowLines(rowLineAlignment* rowLines,int first,int middle, int last){
    int leftSize = middle - first + 1;
    int rightSize = last - middle;

    rowLineAlignment* left = (rowLineAlignment*)malloc(sizeof(rowLineAlignment) * (leftSize+1));
    rowLineAlignment* right = (rowLineAlignment*)malloc(sizeof(rowLineAlignment) * (rightSize+1));

    for(int i = 0; i < leftSize; i++){
        left[i] = rowLines[first+i];
    }
    for(int i = 0; i < rightSize; i++){
        right[i] = rowLines[middle+i+1];
    }

    rowLineAlignment sentinelRowLine = rowLineAlignment(0,0,2147483647,2147483647);

    left[leftSize] = sentinelRowLine;
    right[rightSize] = sentinelRowLine;

    for(int i = 0, j = 0, k = first; k <= last; k++){
        if(left[i].Layer <= right[j].Layer){
            rowLines[k] = left[i];
            i++;
        }
        else{
            rowLines[k] = right[j];
            j++;
        }
    }

    free(left);
    free(right);




}

void mergeSort(int* array, int first, int last){
    if(first < last){
        int middle = (first + last) / 2;
        mergeSort(array,first,middle);
        mergeSort(array,middle+1,last);
        merge(array,first,middle,last);
    }
}


void mergeSort_Pane(uiPane* uiPanes, int first, int last){
    if(first < last){
        int middle = (first + last) / 2;
        mergeSort_Pane(uiPanes,first,middle);
        mergeSort_Pane(uiPanes,middle+1,last);
        merge_Pane(uiPanes,first,middle,last);
    }
}

void mergeSort_RowLines(rowLineAlignment* rowLines,int first, int last){
    if(first < last){
        int middle = (first + last) / 2;
        mergeSort_RowLines(rowLines,first,middle);
        mergeSort_RowLines(rowLines,middle+1,last);
        merge_rowLines(rowLines,first,middle,last);

    }

}



int findNumberOfDigitsInInteger(int x){
    if(x < 0){
        return findNumberOfDigitsInInteger(-x);
    }
    if(x < 10){
        return 1;
    }
    return 1 + findNumberOfDigitsInInteger(x/10);

}

void integerToString(int inputInteger, char* outputString, bool nullTerminated = 1){
    int lastIndexOfOutputString = findNumberOfDigitsInInteger(inputInteger);
    if(nullTerminated == 1){
        outputString[lastIndexOfOutputString] = '\0';
    }
    lastIndexOfOutputString = lastIndexOfOutputString - 1;

    for(lastIndexOfOutputString; inputInteger >= 10; lastIndexOfOutputString--){
        int lastDigit = inputInteger % 10;
        outputString[lastIndexOfOutputString] = lastDigit + 48;
        inputInteger = (inputInteger - lastDigit) / 10;
    }
    outputString[0] = inputInteger + 48;



}







//This function takes in two strings: stringA and stringB
//the function concatenates the two strings to create a new string
char* concatenateStrings(const char* stringA, const char* stringB){
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


char* concatenateStrings2(const char* stringA, const char* stringB, const char* stringC) {
    int stringASize = 0;
    while (stringA[stringASize] != '\0') {
        stringASize++;
    }

    int stringBSize = 0;
    while (stringB[stringBSize] != '\0') {
        stringBSize++;
    }

    int stringCSize = 0;
    while (stringC[stringCSize] != '\0') {
        stringCSize++;
    }

    // Allocate enough memory for all three strings plus the null terminator.
    char* returnString = (char*)malloc((stringASize + stringBSize + stringCSize + 1) * sizeof(char));
    
    // Copy the first string.
    memcpy(returnString, stringA, stringASize * sizeof(char));
    // Copy the second string immediately after the first.
    memcpy(returnString + stringASize, stringB, stringBSize * sizeof(char));
    // Copy the third string after the first two.
    memcpy(returnString + stringASize + stringBSize, stringC, stringCSize * sizeof(char));
    
    // Null terminate the concatenated string.
    returnString[stringASize + stringBSize + stringCSize] = '\0';

    return returnString;
}



void printArray(int* array, int sizeOfArray){
    printf("[");
    for(int i = 0; i < sizeOfArray; i++){
        if((i%10 == 0) && (i != 0)){
            printf("%d\n",array[i]);
        }
        else{
            printf("%d",array[i]);
            printf(",");
        }

    }
    printf("]\n");
}