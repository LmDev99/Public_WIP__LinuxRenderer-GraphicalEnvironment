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
#include "cameraData.hpp"

#define PARENTVOLUME_UNASSIGNED -9999



struct sceneGPUMemoryBlock{
    float memoryStore[16]; //64 bytes
};


struct Material{
    float albedo[4]; //this will determine albedo color, if albedoTextureName is 0 then the value of albedoR will be the textureIndice that corresponds to the albedoTexture for this material
    
    //if any of the following are negative, they will be passed through as absolute value and interpreted as as textureIndice that corresponds to a respective texture
    float metallic; //16 + 4 =20
    float specular; //20 + 4 = 24
    float specularTint; //24 + 4 = 28
    float roughness;    //28 + 4 = 32
    float anistropic; //32 + 4 = 36
    float sheen;  //36 + 4 = 40
    float sheenTint; //40 + 4 = 44
    float clearCoatGloss; //48
    //float endPadding[4];
    bool fieldIsTextureIndex[8];
    float endPadding[2];
    //64Bytes

};



//make it to where you cant create an emissive object that is bounding volume (should be implicit form objectType)
//you also cant add emissive object to a smoothmin()

struct Object{
    //objectType: values 0-99 denote surface/model types (cube,sphere,cone,etc...), values from the range 1000-1099 represent these same surface/model types, 
    //            but with the new indication that they should be treated as emissive objects
    //            values 100-199 represent these same surface/model types but treated as selected objects
    //            The value 1200 (the value for now) is reserved to indicate this is a PRIMARY BOUNDING VOLUME
    int ObjectType; 
    int MaterialIndex; 
    float PositionData[3]; 
    float QuaternionValues[4]; 
    float Scale[3]; 
    float endPadding[4];
    //64 Bytes

    Object(int objectType,int materialIndex, float* positionData, float* quaternionValues, float* scale){
        ObjectType = objectType;
        MaterialIndex = materialIndex;
        if(positionData){
            for(int i = 0; i < 3; i++){
                PositionData[i] = positionData[i];
            }
        }


        if(quaternionValues){
            for(int i = 0; i < 4; i++){
                QuaternionValues[i] = quaternionValues[i];
                
            }
        }


        if(scale){
            for(int i = 0; i < 3; i++){
                Scale[i] = scale[i];

                
            }
        }

    }
    /*
    int intitializeObject(uint32_t id, uint16_t objectType, uint16_t materialIndex, float* positionData, float* quaternionValues, float* scale, float* halfExtents, uint8_t strideToNext, uint16_t parentBoundingVolumeIndex, uint8_t numberOfChildren){
        ID = id;
        ObjectType = objectType;
        MaterialIndex = materialIndex;
        for(int i = 0; i < 3; i++){
            PositionData[i] = positionData[i];
            Scale[i] = scale[i];
            HalfExtents[i] = halfExtents[i];
        }
        for(int i = 0; i < 4; i++){
            QuaternionValues[i] = quaternionValues[i];
        }

        StrideToNext = strideToNext;
        ParentBoundingVolumeIndex = parentBoundingVolumeIndex;
        NumberOfChildren = numberOfChildren;
        
    }
        */

    

};









//the scene is what is loaded into the coherent buffer
class Scene{
    public:
    int objectsNeedSorted;
    int NumberOfSceneGPUMemoryBlocks;

    int MaxNumberOfObjects;
    int numberOfObjects;
    int numberOfNonEmissiveObjects;
    int numberOfEmissiveObjects;
    //int emissiveObjectsIndex; //objectsInScene[emissiveObjectsIndex] to objectsInScene[numberOfObjects-1] are the emissive objects
    Object* objectsInScene;

    
    int numberOfMaterials;
    int materialsOffset;
    Material* materialsInScene;

    Scene(int numberOfSceneGPUMemoryBlocks, int maxNumberOfObjects){
        numberOfObjects = 0;
        numberOfNonEmissiveObjects = 0;
        numberOfEmissiveObjects = 0;
        numberOfMaterials = 0;
        NumberOfSceneGPUMemoryBlocks = numberOfSceneGPUMemoryBlocks;
        MaxNumberOfObjects = maxNumberOfObjects;


    }


    void mergeSortSceneObjects_Merge(int first, int middle, int last){
  
        int leftSize = middle - first + 1;
        int rightSize = last - middle;

        Object* left = (Object*)malloc(sizeof(Object) * (leftSize + 1));
        Object* right = (Object*)malloc(sizeof(Object) * (rightSize + 1));
   
        for(int i = 0; i < leftSize; i++){
            left[i] = objectsInScene[first+i];
        }
        for(int i = 0; i < rightSize; i++){
            right[i] = objectsInScene[middle + i + 1];
        }
    
        Object sentinelObject = Object(2147483647,0,0,0,0);
       
        //Object sentinelObject = {};
        //sentinelObject.ObjectType = 2147483647;
        

        left[leftSize] = sentinelObject;
        right[rightSize] = sentinelObject;
  

        for(int i = 0, j =0, k = first; k <= last; k++){
            if(left[i].ObjectType <= right[j].ObjectType){
                objectsInScene[k] = left[i];
                i++;
            }
            else{
                objectsInScene[k] = right[j];
                j++;
            }



        }
       
        free(left);
        free(right);
    
    }
    void mergeSortSceneObjects(int first, int last){
        
        if(first < last){
            int middle = (first + last) / 2;
          
            mergeSortSceneObjects(first,middle);
            mergeSortSceneObjects(middle+1,last);
            mergeSortSceneObjects_Merge(first,middle,last);

        }


    }




    void sortSceneObjects(){

        mergeSortSceneObjects(0,numberOfObjects-1); //first sort normal and emissive objects 
        

        


        objectsNeedSorted = 0;
    }

    void prepareToRenderScene(){
        if(objectsNeedSorted){
            sortSceneObjects();
          
        }
    }


    int addObject(Object inputObject){
        objectsInScene[numberOfObjects] = inputObject;
        numberOfObjects = numberOfObjects + 1;
        objectsNeedSorted = 1;
        if(inputObject.ObjectType >= 1000){
            numberOfEmissiveObjects = numberOfEmissiveObjects + 1;
        }
        else{
            numberOfNonEmissiveObjects = numberOfNonEmissiveObjects + 1;
        }
        return 0;
    }


/*
    //these are "primary bounding volumes"
    //there are two types:
    //      * "primary composite bounding volume": These are volumes that contain other primary composite volumes OR  primary/secondary encapsulating volumes (remember every normal object
    //                                             is cotnained in its own secondary bounding volume, described by its "half-extents" members)
    //      * "primary encapsulating bounding volume: These are volumes that contain raw sdfs/geometry (meaning that the objects half extents members are ignored/not relevant) "

    //Note that while two encapsulating volumes can overlap, two composite volumes can NEVER overlap, and attempting to add one that overlaps with another will result in ERROR
    int addBoundingBox(Object inputBoundingBox, int* childObjectIDS){

        //first scan through buffer and ensure no other primary composite volumes overlap with this one
        if(inputBoundingBox.ObjectType != 1200){
            return -1;
        }
        
        
        int objectIndicesToMove[inputBoundingBox.NumberOfChildren];
        int numberOfDetectedObjects = 0;
        for(int i = 0 ; i < numberOfNonEmissiveObjects; i++){
            
            if(objectsInScene[i].ID == childObjectIDS[i]){ 
                if(objectsInScene[i].ParentBoundingVolumeIndex != PARENTVOLUME_UNASSIGNED){return -1;}
                //TODO: check that each of the detected objects actually fit inside our bounding box
                objectIndicesToMove[numberOfDetectedObjects] = objectsInScene[i].ID;
                numberOfDetectedObjects = numberOfDetectedObjects + 1;
            }
            
            

        }

        if(numberOfDetectedObjects != (inputBoundingBox.NumberOfChildren)){return -2;}

        
        objectsInScene[numberOfObjects] = inputBoundingBox; 
        numberOfObjects = numberOfObjects + 1;
        objectsNeedSorted = 1;
        for(int i = 0; i < inputBoundingBox.NumberOfChildren; i++){
            objectsInScene[numberOfObjects] = objectsInScene[objectIndicesToMove[i]];
            memcpy(objectsInScene + numberOfObjects, objectsInScene + objectIndicesToMove[i], objectsInScene[objectIndicesToMove[i]].StrideToNext);
            memmove(objectsInScene + objectIndicesToMove[i], objectsInScene + objectIndicesToMove[i] + objectsInScene[objectIndicesToMove[i]].StrideToNext, numberOfObjects - objectIndicesToMove[i]);
            
            numberOfObjects = numberOfObjects + 1;
        }



    }

    */

    void addMaterial(Material inputMaterial){
        materialsInScene[0] = inputMaterial;
        numberOfMaterials = numberOfMaterials + 1;
        materialsInScene = materialsInScene - 1;

    }



};





//TODO
//project is accessed cpu side and is used to load different scenes into the coherent buffer
class project{
    int numberOfScenes;
    int currentlyLoadedSceneIndex;
    Scene* scenesInProject;


    project(){

    }

    //loadScene(){

    //}
    //saveScene(){
        
    //}

};



struct gameDataToRendererPushConstants{
    int renderStage;
    int totalNumberOfObjects;
    int numberOfNonEmissiveObjects;
    int numberOfEmissiveObjects;

    //int emissiveObjectsOffset; thinking of removing this because its redundant, numberOfNonEmissiveObjects can be used to obtain first index of the emissive objects
    int materialsOffset;
    cameraData mainCamera = {};

};