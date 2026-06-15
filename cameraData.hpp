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
#include "input/input.h"
#include "glm/glm.hpp"
void normalizeVector(int sizeOfArray,float* array,float* returnArray){
    float sumOfProducts = 0;
    for(int i = 0; i < sizeOfArray; i++){
        sumOfProducts = sumOfProducts + array[i]*array[i];

    }

    float n = sqrt(sumOfProducts);
    for(int i = 0; i < sizeOfArray; i++){
        returnArray[i] = array[i] / n;

    }


}

float vectorLength(int sizeOfArray, float* array){
    float sumOfSquares = 0;
    for(int i = 0; i < sizeOfArray; i++){
        sumOfSquares = sumOfSquares + array[i]*array[i];

    }

    return sqrt(sumOfSquares);

}

void crossProduct(float* array1, float* array2, float* returnArray){
    returnArray[0] = array1[1]*array2[2] - array1[2]*array2[1];
    returnArray[1] = array1[2]*array2[0] - array1[0]*array2[2];
    returnArray[2] = array1[0]*array2[1] - array1[1]*array2[0];

}
struct cameraData{
    //float[6] header
    float globalUp[3];
    float cameraDirection[3];  //The first 24 bytes (IE globalUP and cameraDirection are not needed for renderer and so they are aliased for use as the input thread systems verificiation header)
    float cameraPosition[3];
    float cameraForward[3];
    float cameraUp[3];
    float cameraRight[3];
    float screenSize[2];
    float fov;
    float quaternionValues[4]; //camera Orientation
    float padding; //padding is so the cameraData is aligned to 8 bytes, which is needed for input system
};

void conjugate(float* quaternionValues, float* returnQuaternion){
    returnQuaternion[0] = quaternionValues[0];
    returnQuaternion[1] = -quaternionValues[1];
    returnQuaternion[2] = -quaternionValues[2];
    returnQuaternion[3] = -quaternionValues[3];
}

void mutliplyQuaternions(float* quaternionValues1, float* quaternionValues2, float* returnQuaternion){
    float w1 = quaternionValues1[0]; float w2 = quaternionValues2[0];
    float x1 = quaternionValues1[1]; float x2 = quaternionValues2[1];
    float y1 = quaternionValues1[2]; float y2 = quaternionValues2[2];
    float z1 = quaternionValues1[3]; float z2 = quaternionValues2[3];


    returnQuaternion[0] = w1 * w2     -      x1 * x2     -      y1 * y2     -       z1 * z2;
    returnQuaternion[1] = w1 * x2     +      x1 * w2     +      y1 * z2     -       z1 * y2;
    returnQuaternion[2] = w1 * y2     -      x1 * z2     +      y1 * w2     +       z1 * x2;
    returnQuaternion[3] = w1 * z2     +      x1 * y2     -      y1 * x2     +       z1 * w2;


}

void getForwardDirection(cameraData* camera,float* returnDirection){
    float vQuat[4] = {0,0,1,0};

    float cameraOrientationConjugate[4];
    conjugate(camera->quaternionValues,cameraOrientationConjugate);

    float tmpOrientation[4];
    mutliplyQuaternions(camera->quaternionValues,vQuat,tmpOrientation);
    float newOrientation[4];
    mutliplyQuaternions(tmpOrientation,cameraOrientationConjugate,newOrientation);

    returnDirection[0]=newOrientation[1];
    returnDirection[1]=newOrientation[2];
    returnDirection[2]=newOrientation[3];

}

void initialize_CameraData(cameraData* camera,int screenWidth, int screenHeight){
    camera->quaternionValues[0] = 1.0f;
    camera->quaternionValues[1] = 0;
    camera->quaternionValues[2] = 0;
    camera->quaternionValues[3] = 0;

    camera->fov = 60;

    camera->cameraPosition[0] = 0.0;
    camera->cameraPosition[1] = -6.0;
    camera->cameraPosition[2] = 0.0;

    camera->globalUp[0] = 0.0;
    camera->globalUp[1] = 0.0;
    camera->globalUp[2] = 1.0;

    camera->screenSize[0] = screenWidth;
    camera->screenSize[1] = screenHeight;
    getForwardDirection(camera,camera->cameraDirection);

    //camera->cameraForward = glm::normalize(camera->cameraDirection);
    normalizeVector(3,camera->cameraDirection,camera->cameraForward);

    float crossProduct1[3] = {0,0,0};
    crossProduct(camera->cameraDirection,camera->globalUp,crossProduct1);
    normalizeVector(3,crossProduct1,camera->cameraRight);

    float crossProduct2[3] = {0,0,0};
    crossProduct(camera->cameraRight,camera->cameraDirection,crossProduct2);
    normalizeVector(3,crossProduct2,camera->cameraUp);

}




float norm(float* quaternionValues){
    float w = quaternionValues[0];
    float x = quaternionValues[1];
    float y = quaternionValues[2];
    float z = quaternionValues[3];
    return sqrt(w*w + x*x + y*y + z*z);
}

void normalizeQuaternion(float* quaternionValues){
    float n = norm(quaternionValues);
    if(n==0){return;}
    quaternionValues[0] = quaternionValues[0] / n;
    quaternionValues[1] = quaternionValues[1] / n;
    quaternionValues[2] = quaternionValues[2] / n;
    quaternionValues[3] = quaternionValues[3] / n;
}


void fromAxisAngle(float axisX, float axisY, float axisZ,float angle,float* returnQuaternion){
    float halfAngle = angle * 0.5f;
    float sinHalfAngle = sin(halfAngle);

    returnQuaternion[0] = cos(halfAngle);
    returnQuaternion[1] = sinHalfAngle * axisX;
    returnQuaternion[2] = sinHalfAngle * axisY;
    returnQuaternion[3] = sinHalfAngle * axisZ;

}










void updateCameraOrientation2(cameraData* camera, controllerState* controller,double deltaTime){
    
    float sensitivity = 0.005f;
    float yaw = -controller->currentRelX * sensitivity; //invert both raw inputs to get an inutive control
    float pitch = -controller->currentRelY * sensitivity;

    

    float qYaw[4] = {0};
    fromAxisAngle(0,0,1,yaw,qYaw);
    float qPitch[4] = {0};
    fromAxisAngle(1,0,0,pitch,qPitch);

    float tmpOrientation[4];
    mutliplyQuaternions(qYaw,camera->quaternionValues,tmpOrientation);

    
    float newOrientation[4];
    mutliplyQuaternions(tmpOrientation,qPitch,newOrientation);

    normalizeQuaternion(newOrientation);

    camera->quaternionValues[0] = newOrientation[0];
    camera->quaternionValues[1] = newOrientation[1];
    camera->quaternionValues[2] = newOrientation[2];
    camera->quaternionValues[3] = newOrientation[3];



    getForwardDirection(camera,camera->cameraDirection);

    
    normalizeVector(3,camera->cameraDirection,camera->cameraForward);

    float crossProduct1[3] = {0,0,0};
    crossProduct(camera->cameraDirection,camera->globalUp,crossProduct1);
    normalizeVector(3,crossProduct1,camera->cameraRight);

    float crossProduct2[3] = {0,0,0};
    crossProduct(camera->cameraRight,camera->cameraDirection,crossProduct2);
    normalizeVector(3,crossProduct2,camera->cameraUp);




    float baseMovementSpeed = 5.0;
    float movementSpeed = baseMovementSpeed * deltaTime;
    float movement[3] = {0.0,0.0,0.0};

    if(controller->keyboardKeysCurrentlyPressed[17]){ //W
       movement[0] = movement[0] + camera->cameraForward[0];
       movement[1] = movement[1] + camera->cameraForward[1];
       movement[2] = movement[2] + camera->cameraForward[2];


    }
    if(controller->keyboardKeysCurrentlyPressed[30]){ //A
        movement[0] = movement[0] - camera->cameraRight[0];
        movement[1] = movement[1] - camera->cameraRight[1];
        movement[2] = movement[2] - camera->cameraRight[2];
        

    }
    if(controller->keyboardKeysCurrentlyPressed[31]){ //S
        movement[0] = movement[0] - camera->cameraForward[0];
        movement[1] = movement[1] - camera->cameraForward[1];
        movement[2] = movement[2] - camera->cameraForward[2];
        
        

    }
    if(controller->keyboardKeysCurrentlyPressed[32]){ //D
        movement[0] = movement[0] + camera->cameraRight[0];
        movement[1] = movement[1] + camera->cameraRight[1];
        movement[2] = movement[2] + camera->cameraRight[2];
       

    }
     // Normalize movement vector to prevent faster diagonal movement
    if (vectorLength(3,movement) > 0.0f) {
        //movement = glm::normalize(movement) * movementSpeed;
        float normalizedMovement[3] = {0.0,0.0,0.0};
        normalizeVector(3,movement,normalizedMovement);
        for(int i = 0; i < 3; i++){
            movement[i] = normalizedMovement[i] * movementSpeed;
        }

    }

    for(int i = 0; i < 3; i++){
        camera->cameraPosition[i] = camera->cameraPosition[i] + movement[i];

    }

}






