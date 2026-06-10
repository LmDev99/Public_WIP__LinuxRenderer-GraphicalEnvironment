/*
 * Copyright (c) 2026 Logan Mottern
 * All rights reserved.
 *
 * This source code is provided for portfolio and review purposes only.
 * No permission is granted to copy, modify, distribute, sublicense, or use
 * this software except with the express written permission of the copyright owner.
 */



//Includes
#define VK_USE_PLATFORM_XCB_KHR

#define RETURN_SUCCESS 0
#define RETURN_FAILURE -1 //any negative value is considered a RETURN_FAILURE not just -1

#include <vulkan/vulkan.h>
#include <xcb/xcb.h>
#include <vulkan/vulkan_xcb.h>

#include <iostream>
#include <cstring> 
#include <unistd.h>
#include <fcntl.h>    

#include <sys/stat.h>
#include "cameraData.hpp"
#include "xcbProcedures.hpp"  
#include "uiStructs5.h"
#include <signal.h>
#include <wait.h>
#include <time.h>
#include "input/input.h"
#include "gameData.hpp"
#include "exrRead.h"
#include "misc.h"   
 
// Function to get current time in seconds
double getCurrentTime() {
    struct timespec now;  
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec / 1'000'000'000.0;
}
    
 

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData){
    if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){
    std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl << std::endl;
    }
    return VK_FALSE;

}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////Small Auxillary Functions/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int clamp(int inputValue, int highBoundary, int lowBoundary){
    if(inputValue < lowBoundary){
        return lowBoundary;
    }
    else if(inputValue > highBoundary){
        return highBoundary;
    }
    else{
        return inputValue;
    }
}



const VkDebugUtilsMessengerCreateInfoEXT vulkanDebugMessengerInfoStruct(const int computationSymbol){
    if(computationSymbol == 1){
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
        debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessengerInfo.pfnUserCallback = debugCallBack;
        return debugMessengerInfo;
    }
    else{
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
        return debugMessengerInfo;

    }

}


const VkDeviceQueueCreateInfo vulkanDeviceQueueInfoStruct(const uint32_t queueFamilyIndex,int queueCount,float* queuePriority){


    VkDeviceQueueCreateInfo infoStruct{};
    infoStruct.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    infoStruct.pNext = nullptr;
    infoStruct.flags = 0;
    infoStruct.queueFamilyIndex = queueFamilyIndex;
    infoStruct.queueCount = queueCount;
    infoStruct.pQueuePriorities = queuePriority;
    return infoStruct;

}

const VkDeviceCreateInfo vulkanDeviceInfoStruct(uint32_t queueCreateInfoCount, VkDeviceQueueCreateInfo* pQueueCreateInfos,int validationLayerCount,char** validationLayerNames,int extensionCount, char** ppEnabledExtensionNames, VkPhysicalDeviceFeatures* pEnabledFeatures){

        VkDeviceCreateInfo infoStruct{};
        infoStruct.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        infoStruct.pNext = nullptr;
        infoStruct.flags = 0;
        infoStruct.queueCreateInfoCount = queueCreateInfoCount;
        infoStruct.pQueueCreateInfos = pQueueCreateInfos;
        infoStruct.enabledLayerCount = validationLayerCount;
        infoStruct.ppEnabledLayerNames = validationLayerNames;
        infoStruct.enabledExtensionCount = extensionCount;
        infoStruct.ppEnabledExtensionNames = ppEnabledExtensionNames;
        infoStruct.pEnabledFeatures = nullptr;
        return infoStruct;
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////PROCEDURES////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




int CREATE_VULKAN_INSTANCE(VkInstance* returnInstancePointer){


   uint32_t extensionCount = 3;
   const char* requiredExtensions[3];
   requiredExtensions[0] = "VK_KHR_surface";
   requiredExtensions[1] = "VK_KHR_xcb_surface";
   requiredExtensions[2] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    const char* layerName1 = "VK_LAYER_KHRONOS_validation";
    const char** validationLayerNames = &layerName1;
    
    VkApplicationInfo appInfo{
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "Public Engine V1",
        VK_MAKE_VERSION(1,0,0),
        "No Engine",
        VK_MAKE_VERSION(1,0,0),
        VK_API_VERSION_1_0
    };

    VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
    debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerInfo.pfnUserCallback = debugCallBack;

    VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = (VkDebugUtilsMessengerEXT*) &debugMessengerInfo;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledExtensionCount = extensionCount;
        instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions;
        instanceCreateInfo.enabledLayerCount = 1;
        instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
    
   

    if(vkCreateInstance(&instanceCreateInfo, nullptr, returnInstancePointer) != VK_SUCCESS){
        return RETURN_FAILURE;
        std::cout << "FAILED TO CREATE INSTANCE" << std::endl;
    }

    return RETURN_SUCCESS;
        
}


int SETUP_DEBUG_MESSENGER(VkInstance instance, VkDebugUtilsMessengerEXT* returnDebugMessengerPointer){
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
        debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessengerInfo.pfnUserCallback = debugCallBack;
        
    //VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = vulkanDebugMessengerInfoStruct(1);


    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(func == nullptr){
        std::cerr << "VK_ERROR_EXTENSION_NOT_PRESENT" << std::endl;
        return RETURN_FAILURE;
    }
    else if(func(instance, &debugMessengerInfo, nullptr, returnDebugMessengerPointer) != VK_SUCCESS){
        std::cerr << "Failed to set up Debug Messenger" << std::endl;
        return RETURN_FAILURE - 1;
    }

    return 0;


}


//This needs to check for the transfer bit as well, not just the compute and graphics bit
//returnArray[0] is the index of the queue family that supports BOTH graphics and compute
//returnArray[1] is the index of the queue family that supports presentation
int GET_FAMILY_QUEUE_INDICES(VkPhysicalDevice physicalDevice, uint32_t availableDeviceCount, VkInstance instance, VkSurfaceKHR surface,/*long int returnArray[][2]*/ long int* returnArray){


    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,queueFamilies);

    returnArray[0] = -1;
    returnArray[1] = -1;
    for(int j = 0; j < queueFamilyCount; j++){

        if((queueFamilies[j].queueFlags & VK_QUEUE_COMPUTE_BIT) && (queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) ){
            returnArray[0] = j;

        }
        VkBool32 presentationSupport = false;
        
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, j, surface, &presentationSupport);
     
        if(presentationSupport){
            returnArray[1] = j;
        }  

        if((returnArray[0] != -1) && (returnArray[1] != -1)){
            return RETURN_SUCCESS;
        }
    }

    return RETURN_FAILURE;




}




//                This is not in use, since I can not test it on current computer

/*
int CREATE_VULKAN_SURFACE_NATIVE(VkInstance instance,VkPhysicalDevice physicalDevice, VkSurfaceKHR* returnSurface, VkExtent2D* returnDisplayModeExtent){
    
      //Select First Display Found
        uint32_t displayCount;
        vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice,&displayCount,nullptr); //this returns the number of displays attached to the selected GPU
        VkDisplayPropertiesKHR displayProperties[displayCount];
        vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice,&displayCount,displayProperties);//this retrives pertaining to each display
        
      //Select First DisplayPlane found
        uint32_t displayPlaneCount;
        vkGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice,&displayPlaneCount,nullptr);
        VkDisplayPlanePropertiesKHR displayPlaneProperties[displayPlaneCount];
        vkGetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice,&displayPlaneCount,displayPlaneProperties);
      //Select First DisplayMode found
        uint32_t displayModeCount;
        vkGetDisplayModePropertiesKHR(physicalDevice,displayProperties[0].display,&displayModeCount,nullptr);
        VkDisplayModePropertiesKHR displayModes[displayModeCount];
        vkGetDisplayModePropertiesKHR(physicalDevice,displayProperties[0].display,&displayModeCount,displayModes);
    

        VkDisplayPlaneCapabilitiesKHR displayPlaneCapabilities{};
        vkGetDisplayPlaneCapabilitiesKHR(physicalDevice,displayModes[0].displayMode,0,&displayPlaneCapabilities);

        //displayPlaneCapabilities.supportedAlpha
        
        //DEBUGGING
        FILE* vulkanPrintOutputFile = fopen("vulkanPrint.txt","w");
        fprintf(vulkanPrintOutputFile,"PRINT display count %d \n PRINT display[0] Plane Count %d"
                 ,displayCount,displayPlaneCount);
        ///



  



    //VkDisplayModeKHR displayMode{};
    uint32_t planeIndex = 0;
    uint32_t planeStackIndex = displayPlaneProperties[planeIndex].currentStackIndex;
    float globalAlpha = 1.0;
    VkExtent2D imageExtent = displayModes[0].parameters.visibleRegion;
    
    
    //VERIFY transform and alphaMode Bit support
    if( ((displayPlaneCapabilities.supportedAlpha & VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR) == 0) || (displayProperties[0].supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR == 0)){
        printf("Tranform Or alphamode bit not supported");
        return -1;
    }



    VkDisplaySurfaceCreateInfoKHR displaySurfaceInfo{};
    displaySurfaceInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
    displaySurfaceInfo.pNext = nullptr;
    displaySurfaceInfo.flags = 0;
    displaySurfaceInfo.displayMode = displayModes[0].displayMode;
    displaySurfaceInfo.planeIndex = planeIndex;
    displaySurfaceInfo.planeStackIndex = planeStackIndex;
    displaySurfaceInfo.transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    displaySurfaceInfo.globalAlpha = globalAlpha;
    displaySurfaceInfo.alphaMode = VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR;
    displaySurfaceInfo.imageExtent = imageExtent;

    vkCreateDisplayPlaneSurfaceKHR(instance,&displaySurfaceInfo,nullptr,returnSurface);
    returnDisplayModeExtent->height = imageExtent.height;
    returnDisplayModeExtent->width = imageExtent.width;
    return 0;

   
}
*/






int GET_VULKAN_PHYSICAL_DEVICE(VkInstance instance,VkSurfaceKHR surface,VkPhysicalDevice* returnSelectedGraphicsCard, uint32_t* returnQueueFamilyIndices){
    //GET ALL AVAILABLE PHYSICAL GPU DEVICES
    uint32_t availableDevicesCount;
    vkEnumeratePhysicalDevices(instance, &availableDevicesCount, nullptr);
    printf("DEBUGG: availableDevicesCount: %d \n",availableDevicesCount);
    VkPhysicalDevice availableDevices[availableDevicesCount];
    vkEnumeratePhysicalDevices(instance, &availableDevicesCount, availableDevices);



    //Select first graphics card that supports requested physical device extensions and queue families
    for(int i = 0; i < availableDevicesCount; i++){



        //Verify that the requested device extensions are available
        bool isSwapChainExtensionSupported = false;

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(availableDevices[i],nullptr,&extensionCount,nullptr);
        VkExtensionProperties allSupportedDeviceExtensions[extensionCount];
        vkEnumerateDeviceExtensionProperties(availableDevices[i],nullptr,&extensionCount, allSupportedDeviceExtensions);

        for(int j = 0; j < extensionCount; j++){
            if(isStringEqual(allSupportedDeviceExtensions[j].extensionName,VK_KHR_SWAPCHAIN_EXTENSION_NAME)){
                isSwapChainExtensionSupported = true;
                printf("DEBUGG: swapchain extension support: %d \n",isSwapChainExtensionSupported);
            }

        }
        


        long int queueIndicesBuffer[2] = {-1, -1};
        if(GET_FAMILY_QUEUE_INDICES(availableDevices[i],availableDevicesCount,instance,surface,queueIndicesBuffer) != RETURN_SUCCESS){
            printf("avalailableDevices[%d] = RETURN_FAILURE \n",i);
        }



        //Verify That the Swap chain is Adequate
        bool swapChainIsAdequate = false;
        if(isSwapChainExtensionSupported == true){

            //Get available surface formats
            uint32_t swapchainFormatCount = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(availableDevices[i], surface, &swapchainFormatCount, nullptr);


            //Get Available Presentation Modes
            uint32_t swapchainPresentModeCount = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(availableDevices[i], surface, &swapchainPresentModeCount, nullptr);


            if((swapchainFormatCount > 0) && (swapchainPresentModeCount > 0)){
                swapChainIsAdequate = true;
            }
        }

       
        //If device[i] is marked as suitable then that device is then returned from the procedure
        if(((queueIndicesBuffer[0] > -1) && (queueIndicesBuffer[1] > -1)) && (isSwapChainExtensionSupported) && (swapChainIsAdequate)){
            printf("thank god \n");
            *returnSelectedGraphicsCard = availableDevices[i];
            returnQueueFamilyIndices[0] = queueIndicesBuffer[0];
            returnQueueFamilyIndices[1] = queueIndicesBuffer[1];
            return RETURN_SUCCESS;
        }

    }

    return RETURN_FAILURE;

}



int GET_VULKAN_LOGICAL_DEVICE(VkPhysicalDevice physicalDevice,uint32_t* queueIndiceData, VkDevice* returnLogicalDevice){
   
    //Create VkDeviceQueueCreateInfo Struct
    VkDeviceQueueCreateInfo queueInfos[2];
    float queuePriority[2] = {1.0f,1.0f};
    int numberOfQueueInfos = 0;
    if(queueIndiceData[0] == queueIndiceData[1]){
        queueInfos[0] = vulkanDeviceQueueInfoStruct(queueIndiceData[0],2,queuePriority);
        numberOfQueueInfos = 1;

    }
    else{
        queueInfos[0] = vulkanDeviceQueueInfoStruct(queueIndiceData[0],1,queuePriority);
        queueInfos[1] = vulkanDeviceQueueInfoStruct(queueIndiceData[1],1,queuePriority);
        numberOfQueueInfos = 2;
    }

    

    //Create VkPhysicalDeviceFeatures Struct
    VkPhysicalDeviceFeatures deviceFeatures = {};
    //Create VkDeviceCreateInfo Struct
    char* layerName1 = "VK_LAYER_KHRONOS_validation";
    char** validationLayerNames = &layerName1;

    char* extensionName1 = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    char** enabledExtensionNames = &extensionName1;
    VkDeviceCreateInfo deviceInfo = vulkanDeviceInfoStruct(numberOfQueueInfos,queueInfos,1,validationLayerNames,1,enabledExtensionNames,&deviceFeatures);
    
    if(vkCreateDevice(physicalDevice,&deviceInfo,nullptr,returnLogicalDevice) != VK_SUCCESS){
        return RETURN_FAILURE;
        std::cout << "failed to create logical device" << std::endl;
    }

    return RETURN_SUCCESS;

}

void GET_VULKAN_QUEUE(VkDevice logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* returnQueue){
    vkGetDeviceQueue(logicalDevice,queueFamilyIndex,queueIndex,returnQueue);
}





int CREATE_VULKAN_SURFACE_XCB(VkInstance instance,xcb_connection_t* xConnection,xcb_window_t windowID,VkSurfaceKHR* returnSurface){


    //vkGetPhysicalDeviceXcbPresentationSupportKHR(physicalDevice, presentationQueueFamilyIndex, xConnection,screen->root_visual);


    VkXcbSurfaceCreateInfoKHR xcbSurfaceInfo = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR, nullptr, 0, xConnection,windowID};

    if(vkCreateXcbSurfaceKHR(instance, &xcbSurfaceInfo, nullptr, returnSurface) != VK_SUCCESS){
        printf("FAILED TO CREATE SURFACE FROM XCB WINDOW \n");
        return RETURN_FAILURE;
    }

    return RETURN_SUCCESS;

   
}






int CREATE_VULKAN_SWAPCHAIN(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, XcbWindow* xcbWindow, uint32_t* queueFamilyIndices, uint32_t* returnImageCount,VkFormat* returnFormat, VkExtent2D* returnExtent, VkSwapchainKHR* returnSwapChain){
    //Get Basic Surface Capabilities
    VkSurfaceCapabilitiesKHR swapchainCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainCapabilities);

    //Get available surface formats
    uint32_t swapchainFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &swapchainFormatCount, nullptr);
    VkSurfaceFormatKHR swapchainFormats[swapchainFormatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &swapchainFormatCount, swapchainFormats);


    //Get Available Presentation Modes
    uint32_t swapchainPresentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &swapchainPresentModeCount, nullptr);
    VkPresentModeKHR swapchainPresentModes[swapchainPresentModeCount];
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &swapchainPresentModeCount, swapchainPresentModes);


    uint32_t imageCount = swapchainCapabilities.minImageCount + 1;
    if((swapchainCapabilities.maxImageCount > 0) && (imageCount > swapchainCapabilities.maxImageCount)){
        imageCount = swapchainCapabilities.maxImageCount;
    }
    if(swapchainCapabilities.maxImageCount >= 3){
        imageCount = 3;
    }

    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = imageCount;
    



    bool requestedFormatFound = 0;
    for(int i = 0; i < swapchainFormatCount; i++){
        if((swapchainFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB) && (swapchainFormats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)){
             swapchainInfo.imageFormat = swapchainFormats[i].format;
             swapchainInfo.imageColorSpace = swapchainFormats[i].colorSpace;
             requestedFormatFound = 1;
        }
    }
    if(requestedFormatFound == 0){
    swapchainInfo.imageFormat = swapchainFormats[0].format;
    swapchainInfo.imageColorSpace = swapchainFormats[0].colorSpace;
    }


    //check if format supports blit operation
    VkFormatProperties formatProperties = {};
    vkGetPhysicalDeviceFormatProperties(physicalDevice,swapchainInfo.imageFormat,&formatProperties);
    if((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) == 0){
        printf("DST BLIT NOT SUPPORTED \n");
        return RETURN_FAILURE;
    }


    bool requestedPresentModeFound = 0;
    for(int i = 0; i < swapchainPresentModeCount; i++){
        if(swapchainPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR){
            swapchainInfo.presentMode = swapchainPresentModes[i];
            requestedPresentModeFound = 1;
        }
    }
    if(requestedPresentModeFound == 0){
        swapchainInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }


    if(swapchainCapabilities.currentExtent.width != UINT32_MAX){
        swapchainInfo.imageExtent = swapchainCapabilities.currentExtent;
    }
    else{
        

        VkExtent2D actualExtent = {xcbWindow->screen->width_in_pixels, xcbWindow->screen->height_in_pixels};
        actualExtent.width = clamp(actualExtent.width, swapchainCapabilities.minImageExtent.width, swapchainCapabilities.maxImageExtent.width);
        actualExtent.height = clamp(actualExtent.height, swapchainCapabilities.minImageExtent.height, swapchainCapabilities.maxImageExtent.height);
        swapchainInfo.imageExtent = actualExtent;

        
    }

    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    if(queueFamilyIndices[0] != queueFamilyIndices[1]){

        swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainInfo.queueFamilyIndexCount = 2;
        swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else{
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.queueFamilyIndexCount = 0;
        swapchainInfo.pQueueFamilyIndices = nullptr;

    }

    swapchainInfo.preTransform = swapchainCapabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.clipped = VK_FALSE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    *returnImageCount = imageCount;
    *returnFormat = swapchainInfo.imageFormat;
    *returnExtent = swapchainInfo.imageExtent;

    if(vkCreateSwapchainKHR(logicalDevice,&swapchainInfo,nullptr,returnSwapChain) != VK_SUCCESS){
        std::cout << "FAILED TO CREATE SWAPCHAIN" << std::endl;
        return RETURN_FAILURE -1;
    }

    vkGetSwapchainImagesKHR(logicalDevice,*returnSwapChain,returnImageCount,nullptr);

    return RETURN_SUCCESS;
    
    
}


int GET_VULKAN_SWAPCHAIN_IMAGES(VkDevice logicalGraphicsCard, VkSwapchainKHR swapChain, uint32_t swapChainImageCount,VkImage* swapChainImages){
    uint32_t tmpSwapChainImageCount = swapChainImageCount;
    if(vkGetSwapchainImagesKHR(logicalGraphicsCard,swapChain,&swapChainImageCount,swapChainImages) != VK_SUCCESS){
        return RETURN_FAILURE;
    }

    return RETURN_SUCCESS;

}      


int CREATE_IMAGE_VIEWS(VkDevice logicalDevice, VkImage* swapChainImages,uint32_t swapChainImageCount, VkFormat swapChainFormat,VkImageView* returnImageViewArray){

    for(int i = 0; i < swapChainImageCount; i++){
        VkImageViewCreateInfo imageViewInfoStruct = {};
        imageViewInfoStruct.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfoStruct.image = swapChainImages[i];
        imageViewInfoStruct.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfoStruct.format = swapChainFormat;
        imageViewInfoStruct.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfoStruct.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfoStruct.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfoStruct.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewInfoStruct.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfoStruct.subresourceRange.baseMipLevel = 0;
        imageViewInfoStruct.subresourceRange.levelCount = 1;
        imageViewInfoStruct.subresourceRange.baseArrayLayer = 0;
        imageViewInfoStruct.subresourceRange.layerCount = 1;
        if(vkCreateImageView(logicalDevice,&imageViewInfoStruct,nullptr,returnImageViewArray + i) != VK_SUCCESS){
            std::cout << "FAILED TO CREATE IMAGE VIEWS" << std::endl;
            return RETURN_FAILURE;
        }
    }

    return RETURN_SUCCESS;



}


int CREATE_SHADER_MODULE(VkDevice logicalGraphicsCard, char* shaderByteCode, int byteCodeSize, VkShaderModule* returnShaderModule){
    VkShaderModuleCreateInfo shaderModuleInfoStruct = {};
    shaderModuleInfoStruct.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfoStruct.codeSize = byteCodeSize;
    shaderModuleInfoStruct.pCode = (uint32_t*)shaderByteCode;

    if(vkCreateShaderModule(logicalGraphicsCard,&shaderModuleInfoStruct,nullptr,returnShaderModule) != VK_SUCCESS){
        std::cout << "FAILED TO CREATE SHADER MODULE" << std::endl;
        return RETURN_FAILURE;
    }

    return RETURN_SUCCESS;
}





int CREATE_MAINRENDERER_COMPUTE_PIPELINE(VkDevice logicalGraphicsCard, VkPipelineLayout* returnPipelineLayout,VkDescriptorSetLayout* returnDescriptorSetLayout, VkPipeline* returnComputePipeline){


    //Compute Shader spirv binary
    int computeShaderFileDescriptor = open("render.spv", O_RDONLY);

    int computeShaderFileSize = GET_FILE_SIZE(computeShaderFileDescriptor);

    char computeShaderBuffer[computeShaderFileSize];

    read(computeShaderFileDescriptor,computeShaderBuffer,computeShaderFileSize);

    VkShaderModule computeShaderModule = {};
    CREATE_SHADER_MODULE(logicalGraphicsCard,computeShaderBuffer,computeShaderFileSize,&computeShaderModule);


 

    VkPipelineShaderStageCreateInfo computeShaderStageInfo = {};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.pNext = nullptr;
    computeShaderStageInfo.flags = 0;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule;
    computeShaderStageInfo.pName = "main";
    computeShaderStageInfo.pSpecializationInfo = nullptr;




    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////Create Descriptor Set Layout///////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkDescriptorSetLayoutBinding computePipelineDescriptorSetBindings[3] = {};

    //render target
    computePipelineDescriptorSetBindings[0].binding = 0;
    computePipelineDescriptorSetBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    computePipelineDescriptorSetBindings[0].descriptorCount = 1;
    computePipelineDescriptorSetBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    computePipelineDescriptorSetBindings[0].pImmutableSamplers = NULL;

    //scene data buffer
    computePipelineDescriptorSetBindings[1].binding = 1;
    computePipelineDescriptorSetBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computePipelineDescriptorSetBindings[1].descriptorCount = 1;
    computePipelineDescriptorSetBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    computePipelineDescriptorSetBindings[1].pImmutableSamplers = NULL;

    //4k textures
    computePipelineDescriptorSetBindings[2].binding = 2;
    computePipelineDescriptorSetBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    computePipelineDescriptorSetBindings[2].descriptorCount = 1;
    computePipelineDescriptorSetBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    computePipelineDescriptorSetBindings[2].pImmutableSamplers = NULL;
  






    VkDescriptorSetLayoutCreateInfo computePipelineDescriptorSetLayoutInfo = {};
    computePipelineDescriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    computePipelineDescriptorSetLayoutInfo.pNext = nullptr;
    computePipelineDescriptorSetLayoutInfo.flags = 0;
    computePipelineDescriptorSetLayoutInfo.bindingCount = 3;
    computePipelineDescriptorSetLayoutInfo.pBindings = computePipelineDescriptorSetBindings;
    if(vkCreateDescriptorSetLayout(logicalGraphicsCard,&computePipelineDescriptorSetLayoutInfo,nullptr,returnDescriptorSetLayout) != VK_SUCCESS){
        printf("FAIL CREATE DESCRIPTOR SET \n");
        return RETURN_FAILURE;
    }


    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT; // Use in compute shader
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(gameDataToRendererPushConstants); // Size of the CameraData structureand other information

    VkPipelineLayoutCreateInfo computePipelineLayoutInfo = {};
    computePipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    computePipelineLayoutInfo.pNext = nullptr;
    computePipelineLayoutInfo.flags = 0;
    computePipelineLayoutInfo.setLayoutCount = 1;
    computePipelineLayoutInfo.pSetLayouts = returnDescriptorSetLayout;
    computePipelineLayoutInfo.pushConstantRangeCount = 1;
    computePipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    
    if(vkCreatePipelineLayout(logicalGraphicsCard,&computePipelineLayoutInfo,nullptr,returnPipelineLayout) != VK_SUCCESS){
        printf("FAIL CREATE PIPELINE LAYOUT \n");
        return RETURN_FAILURE - 1;

    }
 


    VkComputePipelineCreateInfo computePipelineInfo={};
    computePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineInfo.pNext = nullptr;
    computePipelineInfo.flags = 0;
    computePipelineInfo.stage = computeShaderStageInfo;
    computePipelineInfo.layout = *returnPipelineLayout;
    computePipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    computePipelineInfo.basePipelineIndex = -1;

    if(vkCreateComputePipelines(logicalGraphicsCard,nullptr,1,&computePipelineInfo,nullptr,returnComputePipeline) != VK_SUCCESS){
        printf("COULDNT CREATE COMPUTE PIPELINE \n");
        return RETURN_FAILURE - 2;
    }


    vkDestroyShaderModule(logicalGraphicsCard,computeShaderModule,nullptr); //I need to remember why this is the case

    return RETURN_SUCCESS;

}

void CREATE_UIRENDERER_COMPUTE_PIPELINE(VkDevice logicalGraphicsCard, VkPipelineLayout* returnPipelineLayout,VkDescriptorSetLayout* returnDescriptorSetLayout, VkPipeline* returnComputePipeline){


    //Vertex and Fragment Shaders
    int computeShaderFileDescriptor = open("ui.spv", O_RDONLY);

    int computeShaderFileSize = GET_FILE_SIZE(computeShaderFileDescriptor);

    char computeShaderBuffer[computeShaderFileSize];

    read(computeShaderFileDescriptor,computeShaderBuffer,computeShaderFileSize);

    VkShaderModule computeShaderModule = {};
    CREATE_SHADER_MODULE(logicalGraphicsCard,computeShaderBuffer,computeShaderFileSize,&computeShaderModule);




    VkPipelineShaderStageCreateInfo computeShaderStageInfo = {};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.pNext = nullptr;
    computeShaderStageInfo.flags = 0;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule;
    computeShaderStageInfo.pName = "main";
    computeShaderStageInfo.pSpecializationInfo = nullptr;



 

    VkDescriptorSetLayoutBinding computePipelineDescriptorSetBindings[3] = {};

    
    computePipelineDescriptorSetBindings[0].binding = 0;
    computePipelineDescriptorSetBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    computePipelineDescriptorSetBindings[0].descriptorCount = 1;
    computePipelineDescriptorSetBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    computePipelineDescriptorSetBindings[0].pImmutableSamplers = NULL;

    computePipelineDescriptorSetBindings[1].binding = 1;
    computePipelineDescriptorSetBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computePipelineDescriptorSetBindings[1].descriptorCount = 1;
    computePipelineDescriptorSetBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    computePipelineDescriptorSetBindings[1].pImmutableSamplers = NULL;

    computePipelineDescriptorSetBindings[2].binding = 2;
    computePipelineDescriptorSetBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    computePipelineDescriptorSetBindings[2].descriptorCount = 4; // side panel, border visual, bottom panel, glyph atlas
    computePipelineDescriptorSetBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    computePipelineDescriptorSetBindings[2].pImmutableSamplers = NULL;
 



    VkDescriptorSetLayoutCreateInfo computePipelineDescriptorSetLayoutInfo = {};
    computePipelineDescriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    computePipelineDescriptorSetLayoutInfo.pNext = nullptr;
    computePipelineDescriptorSetLayoutInfo.flags = 0;
    computePipelineDescriptorSetLayoutInfo.bindingCount = 3;
    computePipelineDescriptorSetLayoutInfo.pBindings = computePipelineDescriptorSetBindings;
    if(vkCreateDescriptorSetLayout(logicalGraphicsCard,&computePipelineDescriptorSetLayoutInfo,nullptr,returnDescriptorSetLayout) != VK_SUCCESS){
        printf("FAIL CREATE DESCRIPTOR SET \n");
        return;
    }


    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT; // Use in compute shader
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(uiToVulkanPushConstantData); // Size of the CameraData structure

   

    VkPipelineLayoutCreateInfo computePipelineLayoutInfo = {};
    computePipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    computePipelineLayoutInfo.pNext = nullptr;
    computePipelineLayoutInfo.flags = 0;
    computePipelineLayoutInfo.setLayoutCount = 1;
    computePipelineLayoutInfo.pSetLayouts = returnDescriptorSetLayout;
    computePipelineLayoutInfo.pushConstantRangeCount = 1;
    computePipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    
    vkCreatePipelineLayout(logicalGraphicsCard,&computePipelineLayoutInfo,nullptr,returnPipelineLayout);



    VkComputePipelineCreateInfo computePipelineInfo={};
    computePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineInfo.pNext = nullptr;
    computePipelineInfo.flags = 0;
    computePipelineInfo.stage = computeShaderStageInfo;
    computePipelineInfo.layout = *returnPipelineLayout;
    computePipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    computePipelineInfo.basePipelineIndex = -1;

    if(vkCreateComputePipelines(logicalGraphicsCard,nullptr,1,&computePipelineInfo,nullptr,returnComputePipeline) != VK_SUCCESS){
        printf("COULDNT CREATE COMPUTE PIPELINE \n");
        return;
    }


    vkDestroyShaderModule(logicalGraphicsCard,computeShaderModule,nullptr);

}








int ALLOCATE_SINGLE_DESCRIPTOR_SET_SINGLE_POOL_MAINRENDER(int numberOfImages, VkDevice logicalGraphicsCard,VkDescriptorSetLayout* computePipelineDescriptorSetLayout, VkDescriptorPool* returnDescriptorPool,VkDescriptorSet* returnDescriptorSet){
    /*
    //todo
    
    //*/
    VkDescriptorPoolSize descriptorPoolSizes[3] = {};
    descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptorPoolSizes[0].descriptorCount = numberOfImages;

    descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorPoolSizes[1].descriptorCount = 1;

    descriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSizes[2].descriptorCount = 1;



    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.flags = 0; //VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
    descriptorPoolInfo.maxSets = 1;
    descriptorPoolInfo.poolSizeCount = 3;
    descriptorPoolInfo.pPoolSizes = descriptorPoolSizes;
    if(vkCreateDescriptorPool(logicalGraphicsCard, &descriptorPoolInfo,nullptr,returnDescriptorPool) != 0){
        printf("FAILED TO CREATE DESCRIPTOR POOL \n");
        return -1;
    };

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = *returnDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = computePipelineDescriptorSetLayout;
    if(vkAllocateDescriptorSets(logicalGraphicsCard,&descriptorSetAllocateInfo,returnDescriptorSet) != 0){
        printf("FAILED TO ALLOCATE DESCRITPOR SETS \n");
        return -1;
    };

    return 0;



}
 
int ALLOCATE_SINGLE_DESCRIPTOR_SET_SINGLE_POOL2(int numberOfImages, VkDevice logicalGraphicsCard,VkDescriptorSetLayout* computePipelineDescriptorSetLayout,VkDescriptorType descriptorType, VkDescriptorPool* returnDescriptorPool,VkDescriptorSet* returnDescriptorSet){
    
    VkDescriptorPoolSize descriptorPoolSizes[3] = {};
    descriptorPoolSizes[0].type = descriptorType;
    descriptorPoolSizes[0].descriptorCount = numberOfImages; //this is the image we write to and blit into the swapchain image


    descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorPoolSizes[1].descriptorCount = 1;             //this is the storage buffer containing the updateBlockQueue
    descriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSizes[2].descriptorCount = 4;             //these are the three sampled textures one for each of the panes, and the 4th is for the glpyh atlas


    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.flags = 0; //VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
    descriptorPoolInfo.maxSets = 1;
    descriptorPoolInfo.poolSizeCount = 3;
    descriptorPoolInfo.pPoolSizes = descriptorPoolSizes;
    
    if(vkCreateDescriptorPool(logicalGraphicsCard, &descriptorPoolInfo,nullptr,returnDescriptorPool) != 0){
        printf("FAILED TO CREATE DESCRIPTOR POOL \n");
        return RETURN_FAILURE;
    };

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = *returnDescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = computePipelineDescriptorSetLayout;
    if(vkAllocateDescriptorSets(logicalGraphicsCard,&descriptorSetAllocateInfo,returnDescriptorSet) != 0){
        printf("FAILED TO ALLOCATE DESCRITPOR SETS \n");
        return RETURN_FAILURE - 1;
    };

    return RETURN_SUCCESS;



} 



void INITIALIZE_DESCRIPTOR_SET_MAINRENDERER(VkDevice logicalGraphicsCard,VkImageView* computeShaderRenderTarget,VkImageView* mainRenderTextures,VkBuffer* sceneDataBuffer,int sceneDataBufferSize ,VkDescriptorSet descriptorSet){
     VkSamplerCreateInfo mainRenderTextureSamplerCreateInfo  = {};
    


    mainRenderTextureSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    mainRenderTextureSamplerCreateInfo.magFilter = VK_FILTER_LINEAR; 
    mainRenderTextureSamplerCreateInfo.minFilter = VK_FILTER_LINEAR; 
    mainRenderTextureSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; 
    mainRenderTextureSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    mainRenderTextureSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; 
    mainRenderTextureSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; 
    mainRenderTextureSamplerCreateInfo.mipLodBias = 0.0f; 
    mainRenderTextureSamplerCreateInfo.anisotropyEnable = VK_FALSE; 
    mainRenderTextureSamplerCreateInfo.maxAnisotropy = 0; 
    mainRenderTextureSamplerCreateInfo.compareEnable = VK_FALSE; // Enable depth comparison
    mainRenderTextureSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; 
    mainRenderTextureSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE; 
    VkSampler mainRenderTextureSampler = {};
    vkCreateSampler(logicalGraphicsCard,&mainRenderTextureSamplerCreateInfo,nullptr,&mainRenderTextureSampler);


    

    VkDescriptorImageInfo descriptorImageInfoMainRenderTextures = {};
    descriptorImageInfoMainRenderTextures.sampler = mainRenderTextureSampler;
    descriptorImageInfoMainRenderTextures.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptorImageInfoMainRenderTextures.imageView = *mainRenderTextures;




    VkDescriptorImageInfo descriptorImageInfo = {};
    
    descriptorImageInfo.sampler = nullptr;
    descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    descriptorImageInfo.imageView = *computeShaderRenderTarget;

    VkDescriptorBufferInfo descriptorBufferInfo = {};
    descriptorBufferInfo.buffer = *sceneDataBuffer;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = sceneDataBufferSize;
    

    VkWriteDescriptorSet writeDescriptorSets[3] = {};
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = descriptorSet;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].dstArrayElement = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeDescriptorSets[0].pImageInfo = &descriptorImageInfo;
    writeDescriptorSets[0].pBufferInfo = nullptr;
    writeDescriptorSets[0].pTexelBufferView =nullptr;

    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = descriptorSet;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].dstArrayElement = 0;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeDescriptorSets[1].pImageInfo = nullptr;
    writeDescriptorSets[1].pBufferInfo = &descriptorBufferInfo;
    writeDescriptorSets[1].pTexelBufferView =nullptr;


    writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[2].pNext = nullptr;
    writeDescriptorSets[2].dstSet = descriptorSet;
    writeDescriptorSets[2].dstBinding = 2;
    writeDescriptorSets[2].dstArrayElement = 0;
    writeDescriptorSets[2].descriptorCount = 1;

    writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[2].pImageInfo = &descriptorImageInfoMainRenderTextures;
    writeDescriptorSets[2].pBufferInfo = 0;
    writeDescriptorSets[2].pTexelBufferView =nullptr;


    vkUpdateDescriptorSets(logicalGraphicsCard,3, writeDescriptorSets,0,NULL);

}

void INITIALIZE_DESCRIPTOR_SET_UIRENDERER(VkDevice logicalGraphicsCard,VkImageView* computeShaderRenderTarget,VkImageView* uiTextureImageView,int uiToVulkanBufferByteSize, VkBuffer* uiToVulkanBuffer,VkDescriptorSet descriptorSet){

    VkSamplerCreateInfo uiTextureSamplerCreateInfo  = {};
    


    uiTextureSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    uiTextureSamplerCreateInfo.magFilter = VK_FILTER_LINEAR; 
    uiTextureSamplerCreateInfo.minFilter = VK_FILTER_LINEAR; 
    uiTextureSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST; 
    uiTextureSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER; 
    uiTextureSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER; 
    uiTextureSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER; 
    uiTextureSamplerCreateInfo.mipLodBias = 0.0f; 
    uiTextureSamplerCreateInfo.anisotropyEnable = VK_FALSE; 
    uiTextureSamplerCreateInfo.maxAnisotropy = 0; 
    uiTextureSamplerCreateInfo.compareEnable = VK_FALSE; 
    uiTextureSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE; 
    uiTextureSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    VkSampler uiTextureSampler = {};
    vkCreateSampler(logicalGraphicsCard,&uiTextureSamplerCreateInfo,nullptr,&uiTextureSampler);


    

    VkDescriptorImageInfo descriptorImageInfoUiTexturesSets[4] = {};
    descriptorImageInfoUiTexturesSets[0].sampler = uiTextureSampler;
    descriptorImageInfoUiTexturesSets[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptorImageInfoUiTexturesSets[0].imageView = uiTextureImageView[0]; //border texture

    
    descriptorImageInfoUiTexturesSets[1].sampler = uiTextureSampler;
    descriptorImageInfoUiTexturesSets[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptorImageInfoUiTexturesSets[1].imageView = uiTextureImageView[1];//side panel

    descriptorImageInfoUiTexturesSets[2].sampler = uiTextureSampler;
    descriptorImageInfoUiTexturesSets[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptorImageInfoUiTexturesSets[2].imageView = uiTextureImageView[2]; //bottom panel

    descriptorImageInfoUiTexturesSets[3].sampler = uiTextureSampler;
    descriptorImageInfoUiTexturesSets[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    descriptorImageInfoUiTexturesSets[3].imageView = uiTextureImageView[3]; //glyph atlas


    VkDescriptorImageInfo descriptorImageInfoUiRenderTarget = {};
    descriptorImageInfoUiRenderTarget.sampler = nullptr;
    descriptorImageInfoUiRenderTarget.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    descriptorImageInfoUiRenderTarget.imageView = *computeShaderRenderTarget;

    VkDescriptorBufferInfo descriptorBufferInfo = {};
    descriptorBufferInfo.buffer = *uiToVulkanBuffer;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = uiToVulkanBufferByteSize;
    

   

    VkWriteDescriptorSet writeDescriptorSets[3] = {};
    writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[0].pNext = nullptr;
    writeDescriptorSets[0].dstSet = descriptorSet;
    writeDescriptorSets[0].dstBinding = 0;
    writeDescriptorSets[0].dstArrayElement = 0;
    writeDescriptorSets[0].descriptorCount = 1;
    writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeDescriptorSets[0].pImageInfo = &descriptorImageInfoUiRenderTarget;
    writeDescriptorSets[0].pBufferInfo = nullptr;
    writeDescriptorSets[0].pTexelBufferView =nullptr;

    writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[1].pNext = nullptr;
    writeDescriptorSets[1].dstSet = descriptorSet;
    writeDescriptorSets[1].dstBinding = 1;
    writeDescriptorSets[1].dstArrayElement = 0;
    writeDescriptorSets[1].descriptorCount = 1;
    writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeDescriptorSets[1].pImageInfo = nullptr;
    writeDescriptorSets[1].pBufferInfo = &descriptorBufferInfo;
    writeDescriptorSets[1].pTexelBufferView =nullptr;


    writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[2].pNext = nullptr;
    writeDescriptorSets[2].dstSet = descriptorSet;
    writeDescriptorSets[2].dstBinding = 2;
    writeDescriptorSets[2].dstArrayElement = 0; 
    writeDescriptorSets[2].descriptorCount = 4; //0 is border, 1 is side panel, 2 is bottom panel, 3 is glyph atlas
    writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[2].pImageInfo = descriptorImageInfoUiTexturesSets;
    writeDescriptorSets[2].pBufferInfo = 0;
    writeDescriptorSets[2].pTexelBufferView =nullptr;
    
    vkUpdateDescriptorSets(logicalGraphicsCard,3, writeDescriptorSets,0,NULL);

}



void CREATE_COMMAND_POOL(VkDevice logicalGraphicsCard,uint32_t queueFamilyIndex, VkCommandPool* returnCommandPool){
    VkCommandPoolCreateInfo commandPoolInfo = {};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = queueFamilyIndex;

    if(vkCreateCommandPool(logicalGraphicsCard,&commandPoolInfo,nullptr,returnCommandPool) != VK_SUCCESS){
        std::cout << "FAILED TO CREATE COMMAND POOL" << std::endl;
    }

}


void CREATE_COMMAND_BUFFER(VkDevice logicalGraphicsCard,VkCommandPool commandPool,VkCommandBuffer* returnCommandBuffer){

    VkCommandBufferAllocateInfo commandBufferAllocationInfo = {};
    commandBufferAllocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocationInfo.commandPool = commandPool;
    commandBufferAllocationInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocationInfo.commandBufferCount = 1;

    if(vkAllocateCommandBuffers(logicalGraphicsCard,&commandBufferAllocationInfo,returnCommandBuffer) != VK_SUCCESS){
        std::cout << "FAILED TO ALLOCATE COMMAND BUFFERS" << std::endl;
    }

}






void RECORD_COMMAND_BUFFER3(VkCommandBuffer commandBuffer,VkPipeline* computePipelines,VkPipelineLayout* computePipelineLayouts,VkDescriptorSet* descriptorSets,bool swapChainImageFirstPass, VkImage swapChainImage, VkExtent2D swapchainExtent,VkImage* renderTargets, VkExtent2D* renderTargetExtents,VkBuffer uiStorageBuffer,uiData* ui, gameDataToRendererPushConstants* rendererPushConstants, uiToVulkanPushConstantData* uiPushConstantData){


    VkPipeline mainRenderTargetComputePipeline = computePipelines[0];
    VkPipeline uiRenderTargetComputePipeline = computePipelines[1]; 

    VkPipelineLayout mainRenderTargetComputePipelineLayout = computePipelineLayouts[0];
    VkPipelineLayout uiRenderTargetComputePipelineLayout = computePipelineLayouts[1];

    VkDescriptorSet mainRenderTargetDescriptorSet = descriptorSets[0];
    VkDescriptorSet uiRenderTargetDescriptorSet = descriptorSets[1];

    VkImage mainRenderTarget = renderTargets[0];
    VkImage uiRenderTarget = renderTargets[1]; //final composite image

    VkExtent2D mainRenderTargetExtent = renderTargetExtents[0];
    VkExtent2D uiRenderTargetExtent = renderTargetExtents[1];


    
    
 

        // Image dimensions
    
    //uint32_t imageWidth = rendererPushConstants->mainCamera.screenSize.x * 1;
    //uint32_t imageHeight = rendererPushConstants->mainCamera.screenSize.y * 1;
    uint32_t imageWidth = 1920;
    uint32_t imageWidth2 = 1920;
    uint32_t imageHeight = 1080;

    // Workgroup size (should match the shader's local_size_x/y)
    uint32_t workgroupSizeX = 8;
    uint32_t workgroupSizeY = 16;

    // Calculate the number of workgroups
    uint32_t numGroupsX = (imageWidth + workgroupSizeX - 1) / workgroupSizeX;
  
    uint32_t numGroupsX2 = (imageWidth2 + workgroupSizeX - 1) / workgroupSizeX;
    uint32_t numGroupsY = (imageHeight + workgroupSizeY - 1) / workgroupSizeY;


    VkImageMemoryBarrier barrierMainRenderTarget1 = {};
    barrierMainRenderTarget1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierMainRenderTarget1.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrierMainRenderTarget1.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrierMainRenderTarget1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierMainRenderTarget1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierMainRenderTarget1.image = mainRenderTarget;
    barrierMainRenderTarget1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierMainRenderTarget1.subresourceRange.baseMipLevel = 0;
    barrierMainRenderTarget1.subresourceRange.levelCount = 1;
    barrierMainRenderTarget1.subresourceRange.baseArrayLayer = 0;
    barrierMainRenderTarget1.subresourceRange.layerCount = 1;
    barrierMainRenderTarget1.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
    barrierMainRenderTarget1.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    VkImageMemoryBarrier barrierMainRenderTarget2 = {};
    barrierMainRenderTarget2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierMainRenderTarget2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrierMainRenderTarget2.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrierMainRenderTarget2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierMainRenderTarget2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierMainRenderTarget2.image = mainRenderTarget;
    barrierMainRenderTarget2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierMainRenderTarget2.subresourceRange.baseMipLevel = 0;
    barrierMainRenderTarget2.subresourceRange.levelCount = 1;
    barrierMainRenderTarget2.subresourceRange.baseArrayLayer = 0;
    barrierMainRenderTarget2.subresourceRange.layerCount = 1;
    barrierMainRenderTarget2.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrierMainRenderTarget2.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;


    VkImageMemoryBarrier barrierUIRenderTarget1 = {};
    barrierUIRenderTarget1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierUIRenderTarget1.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierUIRenderTarget1.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrierUIRenderTarget1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUIRenderTarget1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUIRenderTarget1.image = uiRenderTarget; // aka final composite image
    barrierUIRenderTarget1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierUIRenderTarget1.subresourceRange.baseMipLevel = 0;
    barrierUIRenderTarget1.subresourceRange.levelCount = 1;
    barrierUIRenderTarget1.subresourceRange.baseArrayLayer = 0;
    barrierUIRenderTarget1.subresourceRange.layerCount = 1;
    barrierUIRenderTarget1.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrierUIRenderTarget1.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

    VkImageMemoryBarrier barrierUIRenderTarget2 = {};
    barrierUIRenderTarget2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierUIRenderTarget2.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrierUIRenderTarget2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrierUIRenderTarget2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUIRenderTarget2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUIRenderTarget2.image = uiRenderTarget; // aka final composite image
    barrierUIRenderTarget2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierUIRenderTarget2.subresourceRange.baseMipLevel = 0;
    barrierUIRenderTarget2.subresourceRange.levelCount = 1;
    barrierUIRenderTarget2.subresourceRange.baseArrayLayer = 0;
    barrierUIRenderTarget2.subresourceRange.layerCount = 1;
    barrierUIRenderTarget2.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    barrierUIRenderTarget2.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    VkImageMemoryBarrier barrierUIRenderTarget3 = {};
    barrierUIRenderTarget3.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierUIRenderTarget3.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrierUIRenderTarget3.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierUIRenderTarget3.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUIRenderTarget3.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUIRenderTarget3.image = uiRenderTarget;
    barrierUIRenderTarget3.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierUIRenderTarget3.subresourceRange.baseMipLevel = 0;
    barrierUIRenderTarget3.subresourceRange.levelCount = 1;
    barrierUIRenderTarget3.subresourceRange.baseArrayLayer = 0;
    barrierUIRenderTarget3.subresourceRange.layerCount = 1;
    barrierUIRenderTarget3.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrierUIRenderTarget3.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  
    
//
    VkImageMemoryBarrier barrierPresent1 = {};
    barrierPresent1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierPresent1.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrierPresent1.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierPresent1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierPresent1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierPresent1.image = swapChainImage;
    barrierPresent1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierPresent1.subresourceRange.baseMipLevel = 0;
    barrierPresent1.subresourceRange.levelCount = 1;
    barrierPresent1.subresourceRange.baseArrayLayer = 0;
    barrierPresent1.subresourceRange.layerCount = 1;
    barrierPresent1.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    barrierPresent1.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    VkImageMemoryBarrier barrierPresent2 = {};
    barrierPresent2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierPresent2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierPresent2.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrierPresent2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierPresent2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierPresent2.image = swapChainImage;
    barrierPresent2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierPresent2.subresourceRange.baseMipLevel = 0;
    barrierPresent2.subresourceRange.levelCount = 1;
    barrierPresent2.subresourceRange.baseArrayLayer = 0;
    barrierPresent2.subresourceRange.layerCount = 1;
    barrierPresent2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrierPresent2.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;


    VkMemoryBarrier barrierUiUpdate = {};
    barrierUiUpdate.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrierUiUpdate.pNext = 0;
    barrierUiUpdate.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    barrierUiUpdate.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VkBufferMemoryBarrier barrierUiStorageBuffer = {};
    barrierUiStorageBuffer.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrierUiStorageBuffer.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT; 
    barrierUiStorageBuffer.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrierUiStorageBuffer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiStorageBuffer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiStorageBuffer.buffer = uiStorageBuffer;
    barrierUiStorageBuffer.offset = 0;
    barrierUiStorageBuffer.size = VK_WHOLE_SIZE;




    VkImageMemoryBarrier barrierUiRenderDispatch = {};
    barrierUiRenderDispatch.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierUiRenderDispatch.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrierUiRenderDispatch.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrierUiRenderDispatch.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiRenderDispatch.dstQueueFamilyIndex =VK_QUEUE_FAMILY_IGNORED;
    barrierUiRenderDispatch.image = uiRenderTarget; //AKA final composite image
    barrierUiRenderDispatch.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierUiRenderDispatch.subresourceRange.baseMipLevel = 0;
    barrierUiRenderDispatch.subresourceRange.levelCount = 1;
    barrierUiRenderDispatch.subresourceRange.baseArrayLayer = 0;
    barrierUiRenderDispatch.subresourceRange.layerCount = 1;
    barrierUiRenderDispatch.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    barrierUiRenderDispatch.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;


     VkImageMemoryBarrier barrierMainRenderDispatch = {};
    barrierMainRenderDispatch.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierMainRenderDispatch.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrierMainRenderDispatch.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrierMainRenderDispatch.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierMainRenderDispatch.dstQueueFamilyIndex =VK_QUEUE_FAMILY_IGNORED;
    barrierMainRenderDispatch.image = mainRenderTarget; 
    barrierMainRenderDispatch.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierMainRenderDispatch.subresourceRange.baseMipLevel = 0;
    barrierMainRenderDispatch.subresourceRange.levelCount = 1;
    barrierMainRenderDispatch.subresourceRange.baseArrayLayer = 0;
    barrierMainRenderDispatch.subresourceRange.layerCount = 10;
    barrierMainRenderDispatch.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    barrierMainRenderDispatch.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;






  

    VkImageBlit blitRegion = {};

    blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; 
    blitRegion.srcSubresource.mipLevel = 0;
    blitRegion.srcSubresource.baseArrayLayer = 0;
    blitRegion.srcSubresource.layerCount = 1; // Blitting one layer
 
    blitRegion.srcOffsets[0] = {0, 0, 0}; // Top-left corner
    blitRegion.srcOffsets[1] = {int(mainRenderTargetExtent.width), int(mainRenderTargetExtent.height), 1}; // Bottom-right corner

    blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blitRegion.dstSubresource.mipLevel = 0;
    blitRegion.dstSubresource.baseArrayLayer = 0;
    blitRegion.dstSubresource.layerCount = 1;

  
    blitRegion.dstOffsets[0] = {ui->gameViewTopLeftPixelX,ui->gameViewTopLeftPixelY,0};
    blitRegion.dstOffsets[1] = {ui->gameViewBottomRightPixelX ,ui->gameViewBottomRightPixelY,1};
 

    VkImageBlit blitRegion2 = {};

    blitRegion2.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; 
    blitRegion2.srcSubresource.mipLevel = 0;
    blitRegion2.srcSubresource.baseArrayLayer = 0;
    blitRegion2.srcSubresource.layerCount = 1; // Blitting one layer

    blitRegion2.srcOffsets[0] = {0, 0, 0}; // Top-left corner
    blitRegion2.srcOffsets[1] = {int(uiRenderTargetExtent.width), int(uiRenderTargetExtent.height), 1}; // Bottom-right corner 

    blitRegion2.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blitRegion2.dstSubresource.mipLevel = 0;
    blitRegion2.dstSubresource.baseArrayLayer = 0;
    blitRegion2.dstSubresource.layerCount = 1;

    blitRegion2.dstOffsets[0] = {0, 0, 0}; // Top-left corner 
    blitRegion2.dstOffsets[1] = {int(swapchainExtent.width), int(swapchainExtent.height), 1}; // Bottom-right corner




    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    if(vkBeginCommandBuffer(commandBuffer,&commandBufferBeginInfo) != VK_SUCCESS){
        std::cout << "FAILED TO BEGIN RECORDING COMMAND BUFFER" << std::endl; 
    }
 
    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_HOST_BIT,         
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,         
        0, nullptr,  
        1, &barrierUiStorageBuffer,    
        0, nullptr    
    );

    for(int i = 0; i < 2; i++){
        rendererPushConstants->renderStage = i;
        vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_COMPUTE,mainRenderTargetComputePipeline);
        vkCmdBindDescriptorSets(commandBuffer,VK_PIPELINE_BIND_POINT_COMPUTE,mainRenderTargetComputePipelineLayout,0,1,&mainRenderTargetDescriptorSet,0,nullptr);
        
        vkCmdPushConstants(commandBuffer, mainRenderTargetComputePipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(gameDataToRendererPushConstants), rendererPushConstants);
        if(i==0){
            vkCmdDispatch(commandBuffer, numGroupsX, numGroupsY, 1);
        }
        else{
            vkCmdPipelineBarrier(
                        commandBuffer,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierMainRenderDispatch
            );
            vkCmdDispatch(commandBuffer, numGroupsX2, numGroupsY, 1);

        }
    }

 
    vkCmdPipelineBarrier(
                        commandBuffer,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierMainRenderTarget1
    );

    //vkCmdBlitImage(commandBuffer,uiRenderTarget,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,swapChainImage,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&blitRegion2,VK_FILTER_LINEAR);
    vkCmdBlitImage(commandBuffer,mainRenderTarget,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,uiRenderTarget,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&blitRegion,VK_FILTER_LINEAR);
    vkCmdPipelineBarrier(
                        commandBuffer,
                        VK_PIPELINE_STAGE_TRANSFER_BIT, 
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierUIRenderTarget1
    );
 
    vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_COMPUTE,uiRenderTargetComputePipeline);
    vkCmdBindDescriptorSets(commandBuffer,VK_PIPELINE_BIND_POINT_COMPUTE,uiRenderTargetComputePipelineLayout,0,1,&uiRenderTargetDescriptorSet,0,nullptr);



    for(int i = 0; i < ui->NumberOfDispatches; i++){
        int totalNumberOfUiUpdateBlockPixels = ui->TotalNumberOfPixelsInDispatch[i];
 
        int uiWorkgroupSize = 128;
        int numberOfUiDispatchGroups = (totalNumberOfUiUpdateBlockPixels + uiWorkgroupSize -1) / 128;
        if(i == 0){
            uiPushConstantData->testValue = 1115;
            uiPushConstantData->updateBlockFirstIndex = ui->DispatchStartIndices[i];
            uiPushConstantData->updateBlockLastIndex = ui->DispatchLastIndices[i];
            vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_COMPUTE,uiRenderTargetComputePipeline);
            vkCmdBindDescriptorSets(commandBuffer,VK_PIPELINE_BIND_POINT_COMPUTE,uiRenderTargetComputePipelineLayout,0,1,&uiRenderTargetDescriptorSet,0,nullptr);
            vkCmdPushConstants(commandBuffer, uiRenderTargetComputePipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(uiToVulkanPushConstantData), uiPushConstantData);
            vkCmdDispatch(commandBuffer, numberOfUiDispatchGroups, 1, 1);
        }
        else{
       
            vkCmdPipelineBarrier(
                                commandBuffer,
                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                0,
                                0,nullptr,
                                0,nullptr,
                                1, &barrierUiRenderDispatch
            );
            
            
            uiPushConstantData->testValue = 99626;
            uiPushConstantData->updateBlockFirstIndex = ui->DispatchStartIndices[i];
            uiPushConstantData->updateBlockLastIndex = ui->DispatchLastIndices[i];
            vkCmdPushConstants(commandBuffer, uiRenderTargetComputePipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(uiToVulkanPushConstantData), uiPushConstantData);
            vkCmdDispatch(commandBuffer, numberOfUiDispatchGroups, 1, 1);

        }
    }
    

    vkCmdPipelineBarrier(
                        commandBuffer,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
                        VK_PIPELINE_STAGE_TRANSFER_BIT, 
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierUIRenderTarget2
    );


    vkCmdPipelineBarrier(
                        commandBuffer,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
                        VK_PIPELINE_STAGE_TRANSFER_BIT, 
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierPresent1
    );

    vkCmdBlitImage(commandBuffer,uiRenderTarget,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,swapChainImage,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&blitRegion2,VK_FILTER_LINEAR);
    
    vkCmdPipelineBarrier(
                        commandBuffer,
                        VK_PIPELINE_STAGE_TRANSFER_BIT, 
                        VK_PIPELINE_STAGE_TRANSFER_BIT, 
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierUIRenderTarget3
    );

    vkCmdPipelineBarrier(
                        commandBuffer,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierPresent2
    );

    vkCmdPipelineBarrier(
                        commandBuffer,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierMainRenderTarget2
    );

    


    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
        std::cout << "FAILED TO RECORD COMMAND BUFFER" << std::endl;
    }


}




void CREATE_SYNC_OBJECTS(VkDevice logicalGraphicsCard,int swapChainImageCount, VkSemaphore* imageAvailableSemaphores, VkSemaphore* renderFinishedSemaphores, VkFence* inFlightFence){
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = 0;//VK_FENCE_CREATE_SIGNALED_BIT;


    if(vkCreateFence(logicalGraphicsCard,&fenceInfo,nullptr,inFlightFence) != VK_SUCCESS){
        std::cout << "FAILED TO CREATE FENCE" << std::endl;
    }

    for(int i = 0; i < swapChainImageCount; i++){
        if(vkCreateSemaphore(logicalGraphicsCard,&semaphoreInfo,nullptr,imageAvailableSemaphores + i) != VK_SUCCESS ||
           vkCreateSemaphore(logicalGraphicsCard,&semaphoreInfo,nullptr,renderFinishedSemaphores + i) != VK_SUCCESS ){
            printf("FAILED TO CREATE SEMAPHORES[%d] \n",i);
        }
    }
}


int CREATE_RENDER_TARGET_IMAGE_AND_VIEW(VkPhysicalDevice physicalGraphicsCard,VkDevice logicalGraphicsCard, VkExtent2D swapChainExtent,VkPhysicalDeviceMemoryProperties* physicalDeviceMemory,VkDeviceMemory* returnImageMemory,VkImage* returnImage,VkExtent2D* returnImageExtent,VkImageView* returnImageView){



    VkImageFormatProperties imageFormatProperties = {};
    if(vkGetPhysicalDeviceImageFormatProperties(physicalGraphicsCard,VK_FORMAT_R32G32B32A32_SFLOAT,VK_IMAGE_TYPE_2D,VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,0,&imageFormatProperties) != VK_SUCCESS){printf(";( \n"); return -1;}

    VkFormatProperties formatProperties = {};
    vkGetPhysicalDeviceFormatProperties(physicalGraphicsCard,VK_FORMAT_R32G32B32A32_SFLOAT,&formatProperties);

    //check for storage support in render target
    if((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) == 0){
        printf("SRC BLIT NOT SUPPORTED \n");
        return -1;
    }
    //check for blit support in render target
    if((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) == 0){
        printf("SRC BLIT NOT SUPPORTED \n");
        return -1;
    }



    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.flags = 0; 
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    
    imageInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    imageInfo.extent = {swapChainExtent.width, swapChainExtent.height, 1};
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 10;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

  
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;



    if(vkCreateImage(logicalGraphicsCard,&imageInfo,nullptr,returnImage)!=VK_SUCCESS){return -1;}


    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalGraphicsCard,*returnImage,&memRequirements);
    

    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < physicalDeviceMemory->memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (physicalDeviceMemory->memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
            memoryTypeIndex = i;
            break;
        }
    }

    if (memoryTypeIndex == UINT32_MAX) {
        // suitable memory type not found
        printf("NO SUITABLE MEMORY TYPE FOUND \n");
        return -1;
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = (memRequirements.size + memRequirements.alignment - 1) & ~(memRequirements.alignment - 1);
    //allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if(vkAllocateMemory(logicalGraphicsCard,&allocInfo,nullptr,returnImageMemory)!=VK_SUCCESS){printf("ERROR \n"); return -1;};

    vkBindImageMemory(logicalGraphicsCard,*returnImage,*returnImageMemory,0);




    VkImageViewCreateInfo imageViewInfoStruct = {};
    imageViewInfoStruct.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfoStruct.image = *returnImage;
    imageViewInfoStruct.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    imageViewInfoStruct.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    imageViewInfoStruct.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfoStruct.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfoStruct.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfoStruct.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imageViewInfoStruct.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfoStruct.subresourceRange.baseMipLevel = 0;
    imageViewInfoStruct.subresourceRange.levelCount = 1;
    imageViewInfoStruct.subresourceRange.baseArrayLayer = 0;
    imageViewInfoStruct.subresourceRange.layerCount = 10;
    if(vkCreateImageView(logicalGraphicsCard,&imageViewInfoStruct,nullptr,returnImageView) != VK_SUCCESS){
        std::cout << "FAILED TO CREATE IMAGE VIEWS" << std::endl;
    }
    //CREATE_IMAGE_VIEWS(logicalGraphicsCard,returnImage,1,VK_FORMAT_R32G32B32A32_SFLOAT,returnImageView);

    *returnImageExtent = {imageInfo.extent.width, imageInfo.extent.height};
    return 0;

}

int CREATE_FINALCOMPOSITE_IMAGE(VkPhysicalDevice physicalGraphicsCard,VkDevice logicalGraphicsCard, VkExtent2D swapChainExtent,VkPhysicalDeviceMemoryProperties* physicalDeviceMemory,VkDeviceMemory* returnImageMemory,VkImage* returnImage, VkImageView* returnImageView){
    VkImageFormatProperties imageFormatProperties = {};
    if(vkGetPhysicalDeviceImageFormatProperties(physicalGraphicsCard,VK_FORMAT_R32G32B32A32_SFLOAT,VK_IMAGE_TYPE_2D,VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT ,0,&imageFormatProperties) != VK_SUCCESS){printf(";( \n"); return -1;}

    VkFormatProperties formatProperties = {};
    vkGetPhysicalDeviceFormatProperties(physicalGraphicsCard,VK_FORMAT_R32G32B32A32_SFLOAT,&formatProperties);

    //check for blit support in render target
    if((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) == 0){
        printf("STORAGE BLIT NOT SUPPORTED \n");
        return -1;
    }
    if((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) == 0){
        printf("SRC BLIT NOT SUPPORTED \n");
        return -1;
    }   
    if((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) == 0){
        printf("DST BLIT NOT SUPPORTED \n");
        return -1;
    }



    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.flags = 0; //VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    
    imageInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    imageInfo.extent = {swapChainExtent.width, swapChainExtent.height, 1};
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;



    if(vkCreateImage(logicalGraphicsCard,&imageInfo,nullptr,returnImage)!=VK_SUCCESS){return -1;}


    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalGraphicsCard,*returnImage,&memRequirements);
    

    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < physicalDeviceMemory->memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (physicalDeviceMemory->memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
            memoryTypeIndex = i;
            break;
        }
    }

    if (memoryTypeIndex == UINT32_MAX) {
        // suitable memory type not found
        printf("NO SUITABLE MEMORY TYPE FOUND \n");
        return -1;
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if(vkAllocateMemory(logicalGraphicsCard,&allocInfo,nullptr,returnImageMemory)!=VK_SUCCESS){printf("ERROR \n"); return -1;};

    vkBindImageMemory(logicalGraphicsCard,*returnImage,*returnImageMemory,0);
    CREATE_IMAGE_VIEWS(logicalGraphicsCard,returnImage,1,VK_FORMAT_R32G32B32A32_SFLOAT,returnImageView);
    
    return 0;
    

}



int CREATE_TEXTURES(VkPhysicalDevice physicalGraphicsCard,VkDevice logicalGraphicsCard, VkExtent2D imageExtent,int numberOfLayers,VkPhysicalDeviceMemoryProperties* physicalDeviceMemory,VkDeviceMemory* returnImageMemory,VkImage* returnImage, VkImageView* returnImageView){
    VkImageFormatProperties imageFormatProperties = {};
    if(vkGetPhysicalDeviceImageFormatProperties(physicalGraphicsCard,VK_FORMAT_R32G32B32A32_SFLOAT,VK_IMAGE_TYPE_2D,VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT ,0,&imageFormatProperties) != VK_SUCCESS){printf(";( \n"); return -1;}

    VkFormatProperties formatProperties = {};
    vkGetPhysicalDeviceFormatProperties(physicalGraphicsCard,VK_FORMAT_R32G32B32A32_SFLOAT,&formatProperties);

    //check for blit support in render target
    if((formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) == 0){
        printf("STORAGE BLIT NOT SUPPORTED \n");
        return -1;
    } 
    if((formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_DST_BIT) == 0){
        printf("DST BLIT NOT SUPPORTED \n");
        return -1;
    }



    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.flags = 0; //VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    
    imageInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    imageInfo.extent = {imageExtent.width, imageExtent.height, 1};
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = numberOfLayers;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;



    if(vkCreateImage(logicalGraphicsCard,&imageInfo,nullptr,returnImage)!=VK_SUCCESS){
    printf("ERROR \n"); return -1;}


    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalGraphicsCard,*returnImage,&memRequirements);
    

    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < physicalDeviceMemory->memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (physicalDeviceMemory->memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
            memoryTypeIndex = i;
            break;
        }
    }

    if (memoryTypeIndex == UINT32_MAX) {
        // suitable memory type not found
        printf("NO SUITABLE MEMORY TYPE FOUND \n");
        return -1;
    }
 
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if(vkAllocateMemory(logicalGraphicsCard,&allocInfo,nullptr,returnImageMemory)!=VK_SUCCESS){printf("ERROR \n"); return -1;};

    vkBindImageMemory(logicalGraphicsCard,*returnImage,*returnImageMemory,0);
    
    VkImageViewCreateInfo imageViewInfoStruct = {};
    imageViewInfoStruct.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfoStruct.image = *returnImage;
    imageViewInfoStruct.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    imageViewInfoStruct.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    imageViewInfoStruct.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfoStruct.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfoStruct.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfoStruct.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imageViewInfoStruct.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfoStruct.subresourceRange.baseMipLevel = 0;
    imageViewInfoStruct.subresourceRange.levelCount = 1;
    imageViewInfoStruct.subresourceRange.baseArrayLayer = 0;
    imageViewInfoStruct.subresourceRange.layerCount = numberOfLayers;
    if(vkCreateImageView(logicalGraphicsCard,&imageViewInfoStruct,nullptr,returnImageView) != VK_SUCCESS){
        std::cout << "FAILED TO CREATE IMAGE VIEWS" << std::endl;
    }


    return 0;
    

}



void CREATE_COHERENT_STORAGE_BUFFER(VkDevice logicalGraphicsCard,VkPhysicalDeviceMemoryProperties physicalDeviceMemory,int bufferSize, VkBuffer* returnStorageBuffer, VkDeviceMemory* returnStorageBufferMemory){
    
   
    //create storage buffer for passing in camera data, next swapchain image index, and other data
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.flags = 0;
    bufferInfo.size = bufferSize ; 
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //used in a single queue
    

    vkCreateBuffer(logicalGraphicsCard, &bufferInfo, nullptr, returnStorageBuffer);



    //allocate the memory that backs our storage buffer. We first query which memory types are valid for our storage buffer to be stored in and check if its adequate
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalGraphicsCard, *returnStorageBuffer, &memRequirements);

    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < physicalDeviceMemory.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (physicalDeviceMemory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            memoryTypeIndex = i;
            break;
        }
    }

    if (memoryTypeIndex == UINT32_MAX) {
        // suitable memory type not found
        printf("NO SUITABLE MEMORY TYPE FOUND \n");
        return;
    }



    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    

    vkAllocateMemory(logicalGraphicsCard,&allocInfo,nullptr,returnStorageBufferMemory);



    //Now that we have a buffer object and have allocated the memory that backs it, we will now bind that object to that memory
    vkBindBufferMemory(logicalGraphicsCard,*returnStorageBuffer,*returnStorageBufferMemory,0);





}  



void CREATE_UI_UPDATEBLOCK_QUEUE_STORAGE_BUFFER(VkDevice logicalGraphicsCard,VkPhysicalDeviceMemoryProperties physicalDeviceMemory,int uiToVulkanBufferByteSize, VkBuffer* returnUniformBuffer, VkDeviceMemory* returnUniformBufferMemory){
    
   
    //create storage buffer for passing in camera data, next swapchain image index, and other data
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.flags = 0;
    bufferInfo.size = uiToVulkanBufferByteSize ; // Size of your storage structure
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //used in a single queue

    vkCreateBuffer(logicalGraphicsCard, &bufferInfo, nullptr, returnUniformBuffer);



    //allocate the memory that backs our storage buffer. We first query which memory types are valid for our storage buffer to be stored in and check if its adequate
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalGraphicsCard, *returnUniformBuffer, &memRequirements);

    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < physicalDeviceMemory.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (physicalDeviceMemory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            memoryTypeIndex = i;
            break;
        }
    }

    if (memoryTypeIndex == UINT32_MAX) {
        // suitable memory type not found
        printf("NO SUITABLE MEMORY TYPE FOUND \n");
        return;
    }



    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    

    vkAllocateMemory(logicalGraphicsCard,&allocInfo,nullptr,returnUniformBufferMemory);



    //Now that we have a buffer object and have allocated the memory that backs it, we will now bind that object to that memory
    vkBindBufferMemory(logicalGraphicsCard,*returnUniformBuffer,*returnUniformBufferMemory,0);





}

int CREATE_STAGINGBUFFER(VkDevice logicalGraphicsCard,VkPhysicalDeviceMemoryProperties physicalDeviceMemory,int bufferByteSize, VkBuffer* returnUniformBuffer, VkDeviceMemory* returnUniformBufferMemory){
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.flags = 0;
    bufferInfo.size = bufferByteSize ; 
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // used in a single queue

    if(vkCreateBuffer(logicalGraphicsCard, &bufferInfo, nullptr, returnUniformBuffer) != VK_SUCCESS){
        printf("FAILED TO CREATE BUFFER \n");
        return RETURN_FAILURE;
    }
    



    //allocate the memory that backs our staging buffer. We first query which memory types are valid for our uniform buffer to be stored in and check if its adequate
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalGraphicsCard, *returnUniformBuffer, &memRequirements);

    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < physicalDeviceMemory.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (physicalDeviceMemory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            memoryTypeIndex = i;
            break;
        }
    }

    if(memoryTypeIndex == UINT32_MAX) {
        // suitable memory type not found
        printf("NO SUITABLE MEMORY TYPE FOUND \n");
        return RETURN_FAILURE - 1;
    }



    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    

    if(vkAllocateMemory(logicalGraphicsCard,&allocInfo,nullptr,returnUniformBufferMemory) != VK_SUCCESS){
        return RETURN_FAILURE - 2;
    }



    //Now that we have a buffer object and have allocated the memory that backs it, we will now bind that object to that memory
    if(vkBindBufferMemory(logicalGraphicsCard,*returnUniformBuffer,*returnUniformBufferMemory,0) != VK_SUCCESS){
        return RETURN_FAILURE - 3;
    }

    return RETURN_SUCCESS;
}

 
void CLEANUP(XcbWindow* xcbWindow,VkInstance instance, VkDevice logicalDevice,VkSurfaceKHR surface, VkSwapchainKHR swapChain, uint32_t swapChainImageCount,VkPipelineLayout pipelineLayout,VkPipeline computePipeline,VkCommandPool commandPool, VkCommandBuffer commandBuffer, VkSemaphore* imageAvailableSemaphores,VkSemaphore* renderFinishedSemaphores,VkFence inFlightFence,VkDescriptorPool descriptorPool,VkDescriptorSetLayout descriptorSetLayout,VkImage renderTarget,VkDeviceMemory renderTargetMemory, VkImageView renderTargetView, VkDebugUtilsMessengerEXT debugMessenger = nullptr){
    printf("DEBUG CLEANUP() 1: \n");
    for(int i = 0; i < swapChainImageCount; i ++){
        vkDestroySemaphore(logicalDevice,imageAvailableSemaphores[i],nullptr);
        vkDestroySemaphore(logicalDevice,renderFinishedSemaphores[i],nullptr);
    }
    printf("DEBUG CLEANUP() 1.5: \n");
    vkDestroyFence(logicalDevice,inFlightFence,nullptr);

     printf("DEBUG CLEANUP() 2: \n");
    vkDestroyDescriptorSetLayout(logicalDevice,descriptorSetLayout,nullptr);
    vkDestroyDescriptorPool(logicalDevice,descriptorPool,nullptr);

    vkDestroyCommandPool(logicalDevice,commandPool,nullptr);

     printf("DEBUG CLEANUP() 3 : \n");
    vkDestroyPipeline(logicalDevice,computePipeline,nullptr);
    printf("DEBUG CLEANUP() 3.2 : \n");
    vkDestroyPipelineLayout(logicalDevice, pipelineLayout,nullptr);
    printf("DEBUG CLEANUP() 3.4 : \n");

    vkDestroyImageView(logicalDevice,renderTargetView,nullptr);
    printf("DEBUG CLEANUP() 3.6 : \n");
    vkDestroyImage(logicalDevice,renderTarget,nullptr);
    printf("DEBUG CLEANUP() 3.8 : \n");
    vkFreeMemory(logicalDevice,renderTargetMemory,nullptr);
    printf("DEBUG CLEANUP() 4: \n");

    vkDestroySwapchainKHR(logicalDevice,swapChain,nullptr);
    printf("DEBUG CLEANUP() 45: \n");
    vkDestroyDevice(logicalDevice, nullptr);
    printf("DEBUG CLEANUP() 475: \n");
    if(debugMessenger != nullptr){
        PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if(func != nullptr){
            func(instance, debugMessenger, nullptr);
        }
    } 
     printf("DEBUG CLEANUP() 5: \n");
    vkDestroySurfaceKHR(instance,surface, nullptr);
     printf("DEBUG CLEANUP() 6: \n");
    vkDestroyInstance(instance, nullptr);
     printf("DEBUG CLEANUP() 7: \n");
    xcb_disconnect(xcbWindow->connection);
     printf("DEBUG CLEANUP() 8: \n");
}


void DRAW_FRAME(VkDevice logicalGraphicsCard,VkQueue computeQueue,VkQueue presentQueue,VkSwapchainKHR swapChain,VkCommandBuffer* commandBuffer,VkPipeline* computePipelines,VkPipelineLayout* computePipelineLayouts,VkDescriptorSet* descriptorSets,VkFence* inFlightFence,VkSemaphore* imageAvailableSemaphores,VkSemaphore* renderFinishedSemaphores,int semaphoreIndex,bool* swapChainImagesFirstPass,VkImage* swapChainImages,VkImage* renderTargets,VkExtent2D swapChainExtent, VkExtent2D* renderTargetExtents,VkBuffer uiStorageBuffer,uiData* ui,gameDataToRendererPushConstants* rendererPushConstants,uiToVulkanPushConstantData* uiPushConstantData){
    //printf("DEBUG FLAGPOST 1 REACHED \n");


      
    
    //printf("DEBUG FLAGPOST 2 REACHED \n");
    uint32_t imageIndex;
    vkAcquireNextImageKHR(logicalGraphicsCard,swapChain,UINT64_MAX,imageAvailableSemaphores[semaphoreIndex],VK_NULL_HANDLE,&imageIndex);
    //printf("DEBUG FLAGPOST 3 REACHED \n");

///* 
    vkResetCommandBuffer(*commandBuffer,0);
    //if(*(ui->UpdateBlockQueueIndex) != 0){
      //  printf("BUFFER 3 HERE! \n");
        RECORD_COMMAND_BUFFER3(*commandBuffer,computePipelines,computePipelineLayouts,descriptorSets,swapChainImagesFirstPass[imageIndex],swapChainImages[imageIndex],swapChainExtent,renderTargets,renderTargetExtents,uiStorageBuffer,ui,rendererPushConstants,uiPushConstantData);
   //}
    //else{
      //  printf("BUFFER 4 HERE! \n");
       // RECORD_COMMAND_BUFFER4(*commandBuffer,computePipelines[0],computePipelineLayouts[0],descriptorSets[0],swapChainImages[imageIndex],swapChainExtent,renderTargets[0],renderTargetExtents[0],camera);
    //}
    //printf("DEBUG FLAGPOST 4 REACHED \n");

    VkSemaphore waitSemaphores[1] = {imageAvailableSemaphores[semaphoreIndex]};
    VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_TRANSFER_BIT};
    VkSubmitInfo queueSubmissionInfo = {};
    queueSubmissionInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    queueSubmissionInfo.waitSemaphoreCount = 1;
    queueSubmissionInfo.pWaitSemaphores = waitSemaphores;
    queueSubmissionInfo.pWaitDstStageMask = waitStages;
    queueSubmissionInfo.commandBufferCount = 1;
    queueSubmissionInfo.pCommandBuffers = commandBuffer;

    VkSemaphore signalSemaphores[1] = {renderFinishedSemaphores[semaphoreIndex]};
    queueSubmissionInfo.signalSemaphoreCount = 1;
    queueSubmissionInfo.pSignalSemaphores = signalSemaphores;

    if(vkQueueSubmit(computeQueue,1,&queueSubmissionInfo,*inFlightFence) != VK_SUCCESS){
        std::cout << "FAILED TO SUBMIT DRAW COMMAND BUFFER" << std::endl;
    }
 
                
 
    VkSwapchainKHR swapChains[1] = {swapChain};
    VkPresentInfoKHR presentationInfo = {};
    presentationInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentationInfo.waitSemaphoreCount = 1;
    presentationInfo.pWaitSemaphores = /*&imageAvailableSemaphore;*/signalSemaphores;
    presentationInfo.swapchainCount = 1;
    presentationInfo.pSwapchains = swapChains;
    presentationInfo.pImageIndices = &imageIndex;
    presentationInfo.pResults = nullptr;

    vkQueuePresentKHR(presentQueue,&presentationInfo);
    vkWaitForFences(logicalGraphicsCard,1,inFlightFence,VK_TRUE,UINT64_MAX);
    vkResetFences(logicalGraphicsCard,1,inFlightFence);
    //vkQueueWaitIdle(computeQueue);

                    // usleep(900000); 
    //usleep(15000);
    swapChainImagesFirstPass[imageIndex] = 0;
 
}    

void INITIALIZE_IMAGE_LAYOUTS(VkDevice logicalGraphicsCard,VkQueue computeQueue,VkCommandBuffer* commandBuffer,VkImage* uiTextureImage,VkImage* mainRenderTextureImage, VkImage* renderTargets,int swapChainImageCount, VkImage* swapChainImages,VkFence* inFlightFence){
    VkImage mainRenderTarget = renderTargets[0];
    VkImage finalCompositionImage = renderTargets[1];

    VkImageMemoryBarrier barrierMainRenderTarget = {};
    barrierMainRenderTarget.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierMainRenderTarget.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierMainRenderTarget.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrierMainRenderTarget.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierMainRenderTarget.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierMainRenderTarget.image = mainRenderTarget;
    barrierMainRenderTarget.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierMainRenderTarget.subresourceRange.baseMipLevel = 0;
    barrierMainRenderTarget.subresourceRange.levelCount = 1;
    barrierMainRenderTarget.subresourceRange.baseArrayLayer = 0;
    barrierMainRenderTarget.subresourceRange.layerCount = 10;
    barrierMainRenderTarget.srcAccessMask = 0;
    barrierMainRenderTarget.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
 
    

    VkImageMemoryBarrier barrierFinalCompositionImage = {};
    barrierFinalCompositionImage.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierFinalCompositionImage.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierFinalCompositionImage.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierFinalCompositionImage.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierFinalCompositionImage.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierFinalCompositionImage.image = finalCompositionImage;
    barrierFinalCompositionImage.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierFinalCompositionImage.subresourceRange.baseMipLevel = 0;
    barrierFinalCompositionImage.subresourceRange.levelCount = 1;
    barrierFinalCompositionImage.subresourceRange.baseArrayLayer = 0;
    barrierFinalCompositionImage.subresourceRange.layerCount = 1;
    barrierFinalCompositionImage.srcAccessMask = 0;
    barrierFinalCompositionImage.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    VkImageMemoryBarrier barrierUiTexturesImage1 = {};
    barrierUiTexturesImage1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierUiTexturesImage1.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierUiTexturesImage1.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierUiTexturesImage1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiTexturesImage1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiTexturesImage1.image = uiTextureImage[0];
    barrierUiTexturesImage1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierUiTexturesImage1.subresourceRange.baseMipLevel = 0;
    barrierUiTexturesImage1.subresourceRange.levelCount = 1;
    barrierUiTexturesImage1.subresourceRange.baseArrayLayer = 0;
    barrierUiTexturesImage1.subresourceRange.layerCount = 1;
    barrierUiTexturesImage1.srcAccessMask = 0;
    barrierUiTexturesImage1.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;



    VkImageMemoryBarrier barrierUiTexturesImage2 = {};
    barrierUiTexturesImage2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierUiTexturesImage2.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierUiTexturesImage2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierUiTexturesImage2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiTexturesImage2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiTexturesImage2.image = uiTextureImage[1];
    barrierUiTexturesImage2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierUiTexturesImage2.subresourceRange.baseMipLevel = 0;
    barrierUiTexturesImage2.subresourceRange.levelCount = 1;
    barrierUiTexturesImage2.subresourceRange.baseArrayLayer = 0;
    barrierUiTexturesImage2.subresourceRange.layerCount = 10;
    barrierUiTexturesImage2.srcAccessMask = 0;
    barrierUiTexturesImage2.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    VkImageMemoryBarrier barrierUiTexturesImage3 = {};
    barrierUiTexturesImage3.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierUiTexturesImage3.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierUiTexturesImage3.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierUiTexturesImage3.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiTexturesImage3.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiTexturesImage3.image = uiTextureImage[2];
    barrierUiTexturesImage3.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierUiTexturesImage3.subresourceRange.baseMipLevel = 0;
    barrierUiTexturesImage3.subresourceRange.levelCount = 1;
    barrierUiTexturesImage3.subresourceRange.baseArrayLayer = 0;
    barrierUiTexturesImage3.subresourceRange.layerCount = 10;
    barrierUiTexturesImage3.srcAccessMask = 0;
    barrierUiTexturesImage3.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    VkImageMemoryBarrier barrierUiTexturesImage4 = {};
    barrierUiTexturesImage4.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierUiTexturesImage4.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierUiTexturesImage4.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierUiTexturesImage4.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiTexturesImage4.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierUiTexturesImage4.image = uiTextureImage[3];
    barrierUiTexturesImage4.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierUiTexturesImage4.subresourceRange.baseMipLevel = 0;
    barrierUiTexturesImage4.subresourceRange.levelCount = 1;
    barrierUiTexturesImage4.subresourceRange.baseArrayLayer = 0;
    barrierUiTexturesImage4.subresourceRange.layerCount = 3;
    barrierUiTexturesImage4.srcAccessMask = 0;
    barrierUiTexturesImage4.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;



    VkImageMemoryBarrier barrierMainRenderTexturesImage1 = {};
    barrierMainRenderTexturesImage1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierMainRenderTexturesImage1.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierMainRenderTexturesImage1.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrierMainRenderTexturesImage1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierMainRenderTexturesImage1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierMainRenderTexturesImage1.image = *mainRenderTextureImage;
    barrierMainRenderTexturesImage1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierMainRenderTexturesImage1.subresourceRange.baseMipLevel = 0;
    barrierMainRenderTexturesImage1.subresourceRange.levelCount = 1;
    barrierMainRenderTexturesImage1.subresourceRange.baseArrayLayer = 0;
    barrierMainRenderTexturesImage1.subresourceRange.layerCount = 3;
    barrierMainRenderTexturesImage1.srcAccessMask = 0;
    barrierMainRenderTexturesImage1.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    

    

    VkImageMemoryBarrier barrierPresentImages[swapChainImageCount] = {};

    for(int i = 0; i < swapChainImageCount; i++){
        barrierPresentImages[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierPresentImages[i].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrierPresentImages[i].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrierPresentImages[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierPresentImages[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierPresentImages[i].image = swapChainImages[i];
        barrierPresentImages[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrierPresentImages[i].subresourceRange.baseMipLevel = 0;
        barrierPresentImages[i].subresourceRange.levelCount = 1;
        barrierPresentImages[i].subresourceRange.baseArrayLayer = 0;
        barrierPresentImages[i].subresourceRange.layerCount = 1;
        barrierPresentImages[i].srcAccessMask = 0;
        barrierPresentImages[i].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    
    }



 

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    

    //BIND STATIC PIPELINE
    if(vkBeginCommandBuffer(*commandBuffer,&commandBufferBeginInfo) != VK_SUCCESS){
        std::cout << "FAILED TO BEGIN RECORDING COMMAND BUFFER" << std::endl;
    }

    vkCmdPipelineBarrier(
                            *commandBuffer,
                            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
                            0,
                            0, nullptr,
                            0, nullptr,
                            1, &barrierMainRenderTarget
        );

    vkCmdPipelineBarrier(
                            *commandBuffer,
                            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                            VK_PIPELINE_STAGE_TRANSFER_BIT, 
                            0,
                            0, nullptr,
                            0, nullptr,
                            1, &barrierFinalCompositionImage
        );
    vkCmdPipelineBarrier(
                        *commandBuffer,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                        VK_PIPELINE_STAGE_TRANSFER_BIT, 
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierUiTexturesImage1
    );
    vkCmdPipelineBarrier(
                        *commandBuffer,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                        VK_PIPELINE_STAGE_TRANSFER_BIT, 
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierUiTexturesImage2
    );
    vkCmdPipelineBarrier(
                        *commandBuffer,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                        VK_PIPELINE_STAGE_TRANSFER_BIT, 
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierUiTexturesImage3
    );
    vkCmdPipelineBarrier(
                        *commandBuffer,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                        VK_PIPELINE_STAGE_TRANSFER_BIT, 
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierUiTexturesImage4
    );
    vkCmdPipelineBarrier(
                        *commandBuffer,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrierMainRenderTexturesImage1
    );
    vkCmdPipelineBarrier(
                            *commandBuffer,
                            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
                            0,
                            0, nullptr,
                            0, nullptr,
                            3, barrierPresentImages
        );

    //vkCmdBindPipeline(*commandBuffer,VK_PIPELINE_BIND_POINT_COMPUTE,computePipeline);
    //vkCmdBindDescriptorSets(*commandBuffer,VK_PIPELINE_BIND_POINT_COMPUTE,computePipelineLayout,0,1,descriptorSet,0,nullptr);

    if(vkEndCommandBuffer(*commandBuffer) != VK_SUCCESS){
        std::cout << "FAILED TO RECORD COMMAND BUFFER" << std::endl;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 0; 
    submitInfo.pWaitSemaphores = NULL; 
    submitInfo.pWaitDstStageMask = NULL; 
    submitInfo.commandBufferCount = 1; 
    submitInfo.pCommandBuffers = commandBuffer; 
    submitInfo.signalSemaphoreCount = 0; 
    submitInfo.pSignalSemaphores = NULL; 

    if(vkQueueSubmit(computeQueue,1,&submitInfo,*inFlightFence) != VK_SUCCESS){
        std::cout << "FAILED TO SUBMIT DRAW COMMAND BUFFER" << std::endl;
    }

    vkWaitForFences(logicalGraphicsCard,1,inFlightFence,VK_TRUE,UINT64_MAX);
    vkResetFences(logicalGraphicsCard,1,inFlightFence);

    

}






//probably rename computeQueue to transferQueue
int loadTexturesMainRender(VkDevice logicalGraphicsCard,VkQueue transferQueue,VkCommandBuffer* commandBuffer,VkImage* mainRendererTextureImages,VkBuffer* stagingBuffer,VkFence* inFlightFence,VkExtent2D mainRenderTextureImageExtent,int baseLayer,int layerCount,int flag=0){
    VkImageMemoryBarrier barrier_TransferOptimalToShaderReadOptimal = {};
    barrier_TransferOptimalToShaderReadOptimal.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier_TransferOptimalToShaderReadOptimal.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier_TransferOptimalToShaderReadOptimal.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier_TransferOptimalToShaderReadOptimal.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier_TransferOptimalToShaderReadOptimal.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier_TransferOptimalToShaderReadOptimal.image = *mainRendererTextureImages;
    barrier_TransferOptimalToShaderReadOptimal.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier_TransferOptimalToShaderReadOptimal.subresourceRange.baseMipLevel = 0;
    barrier_TransferOptimalToShaderReadOptimal.subresourceRange.levelCount = 1;
    barrier_TransferOptimalToShaderReadOptimal.subresourceRange.baseArrayLayer = baseLayer;
    barrier_TransferOptimalToShaderReadOptimal.subresourceRange.layerCount = layerCount;
    barrier_TransferOptimalToShaderReadOptimal.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier_TransferOptimalToShaderReadOptimal.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;




    VkImageMemoryBarrier barrier_ShaderReadOptimalToTransferOptimal = {};
    barrier_ShaderReadOptimalToTransferOptimal.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier_ShaderReadOptimalToTransferOptimal.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier_ShaderReadOptimalToTransferOptimal.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier_ShaderReadOptimalToTransferOptimal.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier_ShaderReadOptimalToTransferOptimal.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier_ShaderReadOptimalToTransferOptimal.image = *mainRendererTextureImages;
    barrier_ShaderReadOptimalToTransferOptimal.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier_ShaderReadOptimalToTransferOptimal.subresourceRange.baseMipLevel = 0;
    barrier_ShaderReadOptimalToTransferOptimal.subresourceRange.levelCount = 1;
    barrier_ShaderReadOptimalToTransferOptimal.subresourceRange.baseArrayLayer = baseLayer;
    barrier_ShaderReadOptimalToTransferOptimal.subresourceRange.layerCount = layerCount;
    barrier_ShaderReadOptimalToTransferOptimal.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier_ShaderReadOptimalToTransferOptimal.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;


    


    /*
    VkClearColorValue clearColor = {{0.06f, 0.002f, 0.02f, 1.0f}};

    
    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;
    */

    VkImageSubresourceLayers mainRendererTexturesSubresource = {};
    mainRendererTexturesSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    mainRendererTexturesSubresource.mipLevel = 0;
    mainRendererTexturesSubresource.baseArrayLayer = baseLayer;
    mainRendererTexturesSubresource.layerCount = layerCount;


    VkOffset3D stagingBufferImageCopyInfoImageOffset = {};
    stagingBufferImageCopyInfoImageOffset.x = 0;
    stagingBufferImageCopyInfoImageOffset.y = 0;
    stagingBufferImageCopyInfoImageOffset.z = 0;



    VkExtent3D stagingBufferImageCopyInfoImageExtent = {};
    stagingBufferImageCopyInfoImageExtent.width = mainRenderTextureImageExtent.width;
    stagingBufferImageCopyInfoImageExtent.height = mainRenderTextureImageExtent.height;
    stagingBufferImageCopyInfoImageExtent.depth = 1;




    VkBufferImageCopy stagingBufferImageCopyInfo = {};
    stagingBufferImageCopyInfo.bufferOffset = 0;
    stagingBufferImageCopyInfo.bufferRowLength = 0;
    stagingBufferImageCopyInfo.bufferImageHeight = 0;
    stagingBufferImageCopyInfo.imageSubresource = mainRendererTexturesSubresource;
    stagingBufferImageCopyInfo.imageOffset = stagingBufferImageCopyInfoImageOffset;
    stagingBufferImageCopyInfo.imageExtent = stagingBufferImageCopyInfoImageExtent;
    
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    

    //BIND STATIC PIPELINE
    if(vkBeginCommandBuffer(*commandBuffer,&commandBufferBeginInfo) != VK_SUCCESS){
        std::cout << "FAILED TO BEGIN RECORDING COMMAND BUFFER" << std::endl;
        return RETURN_FAILURE;
    }
    
        
    
    if(flag==1){
        vkCmdPipelineBarrier(
                                *commandBuffer,
                                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                                VK_PIPELINE_STAGE_TRANSFER_BIT, 
                                0,
                                0, nullptr,
                                0, nullptr,
                                1, &barrier_ShaderReadOptimalToTransferOptimal
                            );
        
    }
        /*
        vkCmdClearColorImage(
        *commandBuffer,
        *uiTextureImage, // The VkImage
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        &clearColor,
        1,
        &subresourceRange
        );
        */
    

    
    

    vkCmdCopyBufferToImage(*commandBuffer,*stagingBuffer,*mainRendererTextureImages,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&stagingBufferImageCopyInfo);
    
   
    
    
    vkCmdPipelineBarrier(
                            *commandBuffer,
                            VK_PIPELINE_STAGE_TRANSFER_BIT, 
                            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
                            0,
                            0, nullptr,
                            0, nullptr,
                            1, &barrier_TransferOptimalToShaderReadOptimal
        );
    
    



    if(vkEndCommandBuffer(*commandBuffer) != VK_SUCCESS){
        std::cout << "FAILED TO RECORD COMMAND BUFFER" << std::endl;
        return RETURN_FAILURE - 1;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 0; 
    submitInfo.pWaitSemaphores = NULL; 
    submitInfo.pWaitDstStageMask = NULL; 
    submitInfo.commandBufferCount = 1; 
    submitInfo.pCommandBuffers = commandBuffer; 
    submitInfo.signalSemaphoreCount = 0; 
    submitInfo.pSignalSemaphores = NULL; 

    if(vkQueueSubmit(transferQueue,1,&submitInfo,*inFlightFence) != VK_SUCCESS){
        std::cout << "FAILED TO SUBMIT COMMAND BUFFER TO QUEUE" << std::endl;

    }

    vkWaitForFences(logicalGraphicsCard,1,inFlightFence,VK_TRUE,UINT64_MAX);
    vkResetFences(logicalGraphicsCard,1,inFlightFence);

    return RETURN_SUCCESS;

}

int loadTexturesUI(VkDevice logicalGraphicsCard,VkQueue transferQueue,VkCommandBuffer* commandBuffer,VkImage* uiTextureImages,VkBuffer* stagingBuffer,VkFence* inFlightFence,VkExtent2D uiTextureImageExtent,int baseLayer,int layerCount,int flag=0){
    VkImageMemoryBarrier barrier_TransferOptimalToShaderReadOptimal = {};
    barrier_TransferOptimalToShaderReadOptimal.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier_TransferOptimalToShaderReadOptimal.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier_TransferOptimalToShaderReadOptimal.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier_TransferOptimalToShaderReadOptimal.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier_TransferOptimalToShaderReadOptimal.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier_TransferOptimalToShaderReadOptimal.image = *uiTextureImages;
    barrier_TransferOptimalToShaderReadOptimal.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier_TransferOptimalToShaderReadOptimal.subresourceRange.baseMipLevel = 0;
    barrier_TransferOptimalToShaderReadOptimal.subresourceRange.levelCount = 1;
    barrier_TransferOptimalToShaderReadOptimal.subresourceRange.baseArrayLayer = baseLayer;
    barrier_TransferOptimalToShaderReadOptimal.subresourceRange.layerCount = layerCount;
    barrier_TransferOptimalToShaderReadOptimal.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier_TransferOptimalToShaderReadOptimal.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;




    VkImageMemoryBarrier barrier_ShaderReadOptimalToTransferOptimal = {};
    barrier_ShaderReadOptimalToTransferOptimal.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier_ShaderReadOptimalToTransferOptimal.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier_ShaderReadOptimalToTransferOptimal.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier_ShaderReadOptimalToTransferOptimal.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier_ShaderReadOptimalToTransferOptimal.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier_ShaderReadOptimalToTransferOptimal.image = *uiTextureImages;
    barrier_ShaderReadOptimalToTransferOptimal.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier_ShaderReadOptimalToTransferOptimal.subresourceRange.baseMipLevel = 0;
    barrier_ShaderReadOptimalToTransferOptimal.subresourceRange.levelCount = 1;
    barrier_ShaderReadOptimalToTransferOptimal.subresourceRange.baseArrayLayer = baseLayer;
    barrier_ShaderReadOptimalToTransferOptimal.subresourceRange.layerCount = layerCount;
    barrier_ShaderReadOptimalToTransferOptimal.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier_ShaderReadOptimalToTransferOptimal.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;


    



    VkImageSubresourceLayers uiTexturesSubresource = {};
    uiTexturesSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    uiTexturesSubresource.mipLevel = 0;
    uiTexturesSubresource.baseArrayLayer = baseLayer;
    uiTexturesSubresource.layerCount = layerCount;


    VkOffset3D stagingBufferImageCopyInfoImageOffset = {};
    stagingBufferImageCopyInfoImageOffset.x = 0;
    stagingBufferImageCopyInfoImageOffset.y = 0;
    stagingBufferImageCopyInfoImageOffset.z = 0;



    VkExtent3D stagingBufferImageCopyInfoImageExtent = {};
    stagingBufferImageCopyInfoImageExtent.width = uiTextureImageExtent.width;
    stagingBufferImageCopyInfoImageExtent.height = uiTextureImageExtent.height;
    stagingBufferImageCopyInfoImageExtent.depth = 1;




    VkBufferImageCopy stagingBufferImageCopyInfo = {};
    stagingBufferImageCopyInfo.bufferOffset = 0;
    stagingBufferImageCopyInfo.bufferRowLength = 0;
    stagingBufferImageCopyInfo.bufferImageHeight = 0;
    stagingBufferImageCopyInfo.imageSubresource = uiTexturesSubresource;
    stagingBufferImageCopyInfo.imageOffset = stagingBufferImageCopyInfoImageOffset;
    stagingBufferImageCopyInfo.imageExtent = stagingBufferImageCopyInfoImageExtent;
    
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    

    //BIND STATIC PIPELINE
    if(vkBeginCommandBuffer(*commandBuffer,&commandBufferBeginInfo) != VK_SUCCESS){
        std::cout << "FAILED TO BEGIN RECORDING COMMAND BUFFER" << std::endl;
        return RETURN_FAILURE;
    }
    
        
    
    if(flag==1){
        vkCmdPipelineBarrier(
                                *commandBuffer,
                                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                                VK_PIPELINE_STAGE_TRANSFER_BIT, 
                                0,
                                0, nullptr,
                                0, nullptr,
                                1, &barrier_ShaderReadOptimalToTransferOptimal
                            );
        
    }
        /*
        vkCmdClearColorImage(
        *commandBuffer,
        *uiTextureImage, // The VkImage
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        &clearColor,
        1,
        &subresourceRange
        );
        */
    

    
    

    vkCmdCopyBufferToImage(*commandBuffer,*stagingBuffer,*uiTextureImages,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&stagingBufferImageCopyInfo);
    
   
    
    
    vkCmdPipelineBarrier(
                            *commandBuffer,
                            VK_PIPELINE_STAGE_TRANSFER_BIT, 
                            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
                            0,
                            0, nullptr,
                            0, nullptr,
                            1, &barrier_TransferOptimalToShaderReadOptimal
        );
    
    



    if(vkEndCommandBuffer(*commandBuffer) != VK_SUCCESS){
        std::cout << "FAILED TO RECORD COMMAND BUFFER" << std::endl;
        return RETURN_FAILURE - 1;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 0; 
    submitInfo.pWaitSemaphores = NULL; 
    submitInfo.pWaitDstStageMask = NULL; 
    submitInfo.commandBufferCount = 1; 
    submitInfo.pCommandBuffers = commandBuffer; 
    submitInfo.signalSemaphoreCount = 0; 
    submitInfo.pSignalSemaphores = NULL;

    if(vkQueueSubmit(transferQueue,1,&submitInfo,*inFlightFence) != VK_SUCCESS){
        std::cout << "FAILED TO SUBMIT COMMAND BUFFER TO QUEUE" << std::endl;

    }

    vkWaitForFences(logicalGraphicsCard,1,inFlightFence,VK_TRUE,UINT64_MAX);
    vkResetFences(logicalGraphicsCard,1,inFlightFence);

    return RETURN_SUCCESS;

}



 


volatile bool running = 1;   
int backgroundThread(void* args){
    printf("Second thread Entered \n");
    sleep(35);
    __atomic_store_n(&running,0,__ATOMIC_RELAXED);
    printf("Second thread concluding \n");
    return 0;
  
}

 
    
 
typedef struct{
    cameraData* cameraStores;
    uiData* ui;
   

}inputThreadInputData;



//12/30
/*
Lets name the four stores as A,B,C,D
when store_Index_Main = 0 we use A,B    and     when store_Index_Main = 1 we use C,D



consider the case in our typing system where the 
-main thread performs its acquire on store_ready_index and yields A and does its switch on store_Index_Main turning 0 to 1
-At the same time the input thread is writing into B


-store_Ready_Index 
the main thread does NOT perform its switch if atomic acquire on store_Read_Index has not changed since the last main iteration


this should now work by changing store_Read_Index no longer be a bool but an integer that describes which of the four stores the ready store is, and not just the local index as given now by the boolean
this way the main thread

*/

volatile int input_mode = 1; //input_mode 1 is ui input with mouse, input_mode 0 is for the game view
volatile uint64_t store_Ready_Epoch = 0; 
volatile bool store_Index_Main = 0;
volatile int data_ready[4] = {0};
volatile bool inputThread_Observes_MainStoreIndex = 0;

volatile uint64_t inputThread_epochState; // the input thread's central epochCounter, this is updated at the start of every inputThread loop iteration, and also containts the input threads observed main switch value
volatile uint64_t epoch_Counters[2+2] = {0}; //one counter for each slot in each of the two banks) counter A is 0,2,4,... counter b i. This is updated during writing into a slots store





volatile int mainRenderScreenWidth = 1920;
volatile int mainRenderScreenHeight = 1080;
volatile bool inputThreadShutDown = false;
volatile bool inputThreadReady = false;

void inputThreadSignalHandler(int signal_number){
    printf("Signal received, cleaning up and exiting...\n");
    __atomic_store_n(&inputThreadShutDown, true, __ATOMIC_RELAXED);
    _exit(0); // Use _exit in a signal handler to exit immediately
}


bool isMainSwitchAcknowledged(bool expectedValue){
    bool inputThreadObserves;
    __atomic_load(&inputThread_Observes_MainStoreIndex,&inputThreadObserves,__ATOMIC_RELAXED);
    return inputThreadObserves == expectedValue;

}

void writeToInputThreadEpochState(uint64_t epochCounter, bool storeIndexMainValue){

}

int inputThread(void* args){

    signal(SIGUSR1,inputThreadSignalHandler);
    usleep(100000);

    inputDevices detectedInputDevices = {};
    identifyInputEventFiles(&detectedInputDevices);


    int mouseEventFileDescriptors[detectedInputDevices.numberOfMouseDevices];
    int keyboardEventFileDescriptors[detectedInputDevices.numberOfKeyboardDevices];

    obtainInputDeviceFileDescriptors(&detectedInputDevices,mouseEventFileDescriptors,keyboardEventFileDescriptors);
 

    pollfd pollFileDescriptors[detectedInputDevices.numberOfMouseDevices+detectedInputDevices.numberOfKeyboardDevices];
    createPollFileDescriptors(pollFileDescriptors,detectedInputDevices.numberOfMouseDevices,mouseEventFileDescriptors,detectedInputDevices.numberOfKeyboardDevices,keyboardEventFileDescriptors);
    


    cameraData inputThreadCamera = {};
    initialize_CameraData(&inputThreadCamera,mainRenderScreenWidth,mainRenderScreenHeight);

    mouseCoordinates inputMouse = {};  
    inputMouse.mousePixelCoordinateX = 1920/2;      
    inputMouse.mousePixelCoordinateY = 1080/2;
  
    mouseButtons inputMouseButtons = {}; 
    inputMouseButtons.buttons[0] = 0; inputMouseButtons.buttons[1] = 0; inputMouseButtons.buttons[2] = 0; 
        
    __atomic_store_n(&inputThreadReady,1,__ATOMIC_RELAXED);

    //mouseCoordinates inputThreadMouse = {};
    //inputThreadMouse.mousePixelCoordinateX = screenWidth/2; //this should match how the ui object initializes the mouse
    //inputThreadMouse.mousePixelCoordinateY = screenHeight/2; //this should match how the ui object initializes the mouse

    inputThreadInputData inputData = *((inputThreadInputData*)(args));

    
    bool storeIndexMain = false;
    bool storeIndexInputThread = false;

    bool lastStoreIndexMain = false; //
    //int fileDescriptor = open("/dev/input/mice", O_RDONLY);
    RecordedKeyPresses keyPressRecordList;
    keyPressRecordList.initializeRecordedKeyPresses(2000);
          
    
    controllerState controller = {};     
    bool inputThreadFirstPass = 1;
    bool inputThread_KeyPressRecord_NeedsCleared = 0;  

    uint64_t epochCounter = 0;
    writeToInputThreadEpochState(epochCounter, storeIndexMain);
   
    double previousTime = getCurrentTime();


    while(running){ 
        if(!inputThreadFirstPass){
            epochCounter = epochCounter + 1;
        }

        
             
        double currentTime = getCurrentTime(); 
        double deltaTime = currentTime - previousTime;
        previousTime = currentTime;
        rawInputV3(pollFileDescriptors,detectedInputDevices.numberOfMouseDevices,mouseEventFileDescriptors,detectedInputDevices.numberOfKeyboardDevices,keyboardEventFileDescriptors,&controller,currentTime);
        
        /*
        printf("INPUT raw delta: relX=%d relY=%d buttons=%d %d %d input_mode=%d\n",
       controller.currentRelX,
       controller.currentRelY,
       controller.mouseKeysCurrentlyPressed[0],
       controller.mouseKeysCurrentlyPressed[1],
       controller.mouseKeysCurrentlyPressed[2],
       input_mode);
fflush(stdout);
*/

        if(inputThread_KeyPressRecord_NeedsCleared == 1){ keyPressRecordList.initializeRecordedKeyPresses(2000,1);inputThread_KeyPressRecord_NeedsCleared = 0; }
        keyPressRecordList.scanController(&controller,currentTime);
        //printControllerState(&controller);
        //printf("\n\n\n\n\n");
        
        
                     
      
        if(inputData.ui->isMouseInGameViewPane() && controller.mouseKeysCurrentlyPressed[1]){
          
            __atomic_store_n(&input_mode,0,__ATOMIC_RELAXED); 
        }
        else{  
            __atomic_store_n(&input_mode,1,__ATOMIC_RELAXED);
   
        }
        if(input_mode == 0){
            updateCameraOrientation2(&inputThreadCamera,&controller,deltaTime);
        }
        
       
        
        
        __atomic_load(&store_Index_Main,&storeIndexMain,__ATOMIC_RELAXED);
        int dataIndex = (2 * storeIndexMain) + (epochCounter%2);
        //printf("INPUT THREAD: cameraStoreIndexMain = %d \n",cameraStoreIndexMain);
        //printf("INPUT THREAD: cameraStoreIndexInputThread = %d \n",cameraStoreIndexInputThread);
        
        __atomic_store_n(data_ready + dataIndex, -1, __ATOMIC_RELAXED);
         __atomic_store_n(epoch_Counters + dataIndex, epochCounter,__ATOMIC_RELAXED);
         

 

       
        if(storeIndexMain != lastStoreIndexMain){
            lastStoreIndexMain = storeIndexMain;
            
             __atomic_store_n(&inputThread_Observes_MainStoreIndex,storeIndexMain,__ATOMIC_RELAXED);
            
            
            inputThread_KeyPressRecord_NeedsCleared = 1; ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
        }
        
        uint64_t markerSignature[3] = {epochCounter,epochCounter,epochCounter};

        if(input_mode == 0){
            memcpy(inputData.cameraStores + dataIndex,&inputThreadCamera,sizeof(cameraData));
        }  
        else if(input_mode == 1){

           
            inputData.ui->processMouseUpdate2(controller.currentRelX,-controller.currentRelY,&inputMouse);
            inputMouse.InputSystemHeaderMarkers[0] = epochCounter;   inputMouse.InputSystemHeaderMarkers[1] = epochCounter;   inputMouse.InputSystemHeaderMarkers[2] = epochCounter;
            inputMouse.InputSystemFootnoteMarkers[0] = epochCounter; inputMouse.InputSystemFootnoteMarkers[1] = epochCounter; inputMouse.InputSystemFootnoteMarkers[2] = epochCounter;
            memcpy(inputData.ui->uiMouse.mouseXY_Stores + dataIndex, &inputMouse, sizeof(mouseCoordinates));
            
                        

            memcpy(inputData.ui->KeyPressRecordStores + (dataIndex * inputData.ui->KeyPressRecordBufferSize), markerSignature, 24);
            inputData.ui->KeyPressRecordStores[(dataIndex * inputData.ui->KeyPressRecordBufferSize) + 1].paddingForInputHeader = keyPressRecordList.NumberOfRecordedEntriesInPass;
            memcpy(inputData.ui->KeyPressRecordStores + ((dataIndex * inputData.ui->KeyPressRecordBufferSize)+2),  keyPressRecordList.RecordedKeyPresses, sizeof(KeyPressRecord) * keyPressRecordList.NumberOfRecordedEntriesInPass);
            memcpy( ( ((char*)(inputData.ui->KeyPressRecordStores + ((dataIndex * inputData.ui->KeyPressRecordBufferSize)+ 2))) + (sizeof(KeyPressRecord) * keyPressRecordList.NumberOfRecordedEntriesInPass)), markerSignature, 24);

            

            inputMouseButtons.buttons[0] = controller.mouseKeysCurrentlyPressed[0];
            inputMouseButtons.buttons[1] = controller.mouseKeysCurrentlyPressed[1];
            inputMouseButtons.buttons[2] = controller.mouseKeysCurrentlyPressed[2];
            inputMouseButtons.InputSystemHeaderMarkers[0] = epochCounter; inputMouseButtons.InputSystemHeaderMarkers[1] = epochCounter; inputMouseButtons.InputSystemHeaderMarkers[2] = epochCounter;
            inputMouseButtons.InputSystemFootnoteMarkers[0] = epochCounter; inputMouseButtons.InputSystemFootnoteMarkers[1] = epochCounter; inputMouseButtons.InputSystemFootnoteMarkers[2] = epochCounter;
            memcpy((inputData.ui->uiMouse.mouseKeys_Stores + dataIndex), &inputMouseButtons,sizeof(mouseButtons));
           
              
        } 
       


        __atomic_store_n(data_ready + dataIndex, 1, __ATOMIC_RELAXED);
        __atomic_store_n(&store_Ready_Epoch, epochCounter, __ATOMIC_RELEASE);
        inputThreadFirstPass = 0; 

        
    }

    __atomic_store_n(&inputThreadShutDown, true, __ATOMIC_RELAXED);
    return 0;
    
} 
  
    
  
 
bool verifyMarker(uint64_t* markerPointer, uint64_t markerValue){
    printf("%d == %d     &&     %d == %d     &&     %d == %d  \n",markerPointer[0], markerValue, markerPointer[1], markerValue, markerPointer[2], markerValue);
    if(markerPointer[0] == markerValue && markerPointer[1] == markerValue && markerPointer[2] == markerValue){
        return 1;
    }
    else{
        return 0;
    }
}




/*

The main thread is the renderer and as such is the thread the communicates through vulkan to the gpu for executing our compute shader programs used for rendering
In a general overview the initialization process of establishing this communication is as follows

- Create Vulkan instance 
- select physical device (gpu) 
- create Logical device 
- create compute and presentation queues 
- create and setup main render compute pipeline 
- create and setup ui render compute pipeline



For now the renderer displays to the screen utilizing vulkan presentation extensions via the swapchain and a XCB exposed Surface, However later it is planned
for the renderer to display through the linux drm/kms and utilize vulkan as a headless renderer.


*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////Program Entry Point///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int main(){





    VkInstance instance = {};
    CREATE_VULKAN_INSTANCE(&instance);


    VkDebugUtilsMessengerEXT debugMessenger = {};
    SETUP_DEBUG_MESSENGER(instance, &debugMessenger);

 
    
     
    XcbWindow xcbWindow = {};
    CREATE_XCB_WINDOW(800,600,1,&xcbWindow);

    DISPLAY_XCB_WINDOW(&xcbWindow);

    VkSurfaceKHR surface = {};
    if(CREATE_VULKAN_SURFACE_XCB(instance,xcbWindow.connection,xcbWindow.window,&surface) != 0){printf("FAILED TO CREATE SURFACE \n"); return -1;};

 
    VkPhysicalDevice selectedGraphicsCard = {};
    uint32_t queueFamilyIndices[2];
    GET_VULKAN_PHYSICAL_DEVICE(instance,surface,&selectedGraphicsCard,queueFamilyIndices);


    VkPhysicalDeviceMemoryProperties physicalDeviceMemory = {};
    vkGetPhysicalDeviceMemoryProperties(selectedGraphicsCard,&physicalDeviceMemory);



    printf("%d and %d \n",queueFamilyIndices[0],queueFamilyIndices[1]);
    VkDevice logicalGraphicsCard = {};
    GET_VULKAN_LOGICAL_DEVICE(selectedGraphicsCard, queueFamilyIndices,&logicalGraphicsCard);

 


    VkQueue computeQueue = {};
    VkQueue presentationQueue = {};
    if(queueFamilyIndices[0] == queueFamilyIndices[1]){
        GET_VULKAN_QUEUE(logicalGraphicsCard, queueFamilyIndices[0], 0, &computeQueue);
        GET_VULKAN_QUEUE(logicalGraphicsCard, queueFamilyIndices[1],1, &presentationQueue);

    }
    else{
        GET_VULKAN_QUEUE(logicalGraphicsCard, queueFamilyIndices[0], 0, &computeQueue);
        GET_VULKAN_QUEUE(logicalGraphicsCard, queueFamilyIndices[1],0, &presentationQueue);

    }

     

    VkSwapchainKHR swapChain = {};
    uint32_t swapChainImageCount = 0;
    VkExtent2D swapChainExtent = {};
    VkFormat swapChainFormat = {};
    CREATE_VULKAN_SWAPCHAIN(selectedGraphicsCard,logicalGraphicsCard,surface,&xcbWindow,queueFamilyIndices,&swapChainImageCount,&swapChainFormat,&swapChainExtent,&swapChain);

    if(swapChainImageCount != 3){
        printf("TRIPLE BUFFER SWAPCHAIN NOT ENABLED \n");
        return RETURN_FAILURE;
    }

    VkImage swapChainImages[swapChainImageCount];
    GET_VULKAN_SWAPCHAIN_IMAGES(logicalGraphicsCard,swapChain,swapChainImageCount,swapChainImages);



  

  
////////////SETUP MAIN RENDER COMPUTE PIPELINE/////////////////////////////////////////////

  
    VkPipelineLayout computePipelineLayout = {};
    VkPipeline computePipeline = {};
    VkDescriptorSetLayout computePipelineDescriptorSetLayout = {};
    CREATE_MAINRENDERER_COMPUTE_PIPELINE(logicalGraphicsCard,&computePipelineLayout,&computePipelineDescriptorSetLayout,&computePipeline);

    
 
    VkDescriptorPool descriptorPool = {};
    VkDescriptorSet descriptorSet = {};
    ALLOCATE_SINGLE_DESCRIPTOR_SET_SINGLE_POOL_MAINRENDER(1,logicalGraphicsCard,&computePipelineDescriptorSetLayout,&descriptorPool,&descriptorSet);
 
    

    //int numberOfSceneGPUMemoryBlocks = 312500;
    Scene defualtStartupScene = Scene(312500,10);
    int sceneMemoryAllocationSize = sizeof(sceneGPUMemoryBlock) * defualtStartupScene.NumberOfSceneGPUMemoryBlocks; //15mb
    VkBuffer sceneDataStorageBuffer = {};
    VkDeviceMemory sceneDataStorageBufferMemory = {};
    CREATE_COHERENT_STORAGE_BUFFER(logicalGraphicsCard,physicalDeviceMemory,sceneMemoryAllocationSize,&sceneDataStorageBuffer,&sceneDataStorageBufferMemory);

    
           
    void* sceneRawData;
    vkMapMemory(logicalGraphicsCard,sceneDataStorageBufferMemory,0,sceneMemoryAllocationSize,0,&sceneRawData);
    
   

    defualtStartupScene.objectsInScene = (Object*)sceneRawData;
    float defaultCubePositionData[3] = {0,0,0};
    float defaultCubeQuaternionData[4] = {0,0,0,0};
    float defaultCubeScale [3] = {2,2,2};

    
           
    Object defaultCube = Object(0,0,defaultCubePositionData,defaultCubeQuaternionData,defaultCubeScale);

  

    float defaultCubePositionData2[3] = {0,0,4};
    float defaultCubeQuaternionData2[4] = {0,0,0,0};
    float defaultCubeScale2 [3] = {2,2,2};
           
    Object defaultCube2 = Object(0,0,defaultCubePositionData2,defaultCubeQuaternionData2,defaultCubeScale2);

 

    float defaultEmissiveSpherePositionData[3] = {2,-3,3};
    float defaultEmissiveSphereQuaternionData[4] = {0,0,0,0}; 
    float defaultEmissiveSphereScale[3] = {2,2,2};

   
    Object defaultEmissiveSphere = Object(1001,1,defaultEmissiveSpherePositionData,defaultEmissiveSphereQuaternionData,defaultEmissiveSphereScale);

    

     float defaultEmissiveSpherePositionData2[3] = {1,4,6};
    float defaultEmissiveSphereQuaternionData2[4] = {0,0,0,0}; 
    float defaultEmissiveSphereScale2[3] = {2,2,2};

  
    Object defaultEmissiveSphere2 = Object(1001,2,defaultEmissiveSpherePositionData2,defaultEmissiveSphereQuaternionData2,defaultEmissiveSphereScale2);

  
    

    Material defaultCubeMaterial = {};
    defaultCubeMaterial.albedo[0] = 0.5; defaultCubeMaterial.albedo[1] = 0.7; defaultCubeMaterial.albedo[2] = 0.5; defaultCubeMaterial.albedo[3] = 1.0;
    defaultCubeMaterial.metallic = 1.0;
    defaultCubeMaterial.specular = 0.6;
    defaultCubeMaterial.specularTint = 0.0;
    defaultCubeMaterial.roughness = 0.15;
    defaultCubeMaterial.anistropic = 0.0;
    defaultCubeMaterial.sheen = 0.0;
    defaultCubeMaterial.sheenTint = 0.0;
    
    
 

 

  
 
    Material defaultEmissiveSphereMaterial = {};
    defaultEmissiveSphereMaterial.albedo[0] = 25.0; defaultEmissiveSphereMaterial.albedo[1] = 12.5; defaultEmissiveSphereMaterial.albedo[2] = 12.5; defaultEmissiveSphereMaterial.albedo[3] = 1.0;
    defaultEmissiveSphereMaterial.metallic = -1.0;
    defaultEmissiveSphereMaterial.specular = -1.0;
    defaultEmissiveSphereMaterial.specularTint = -1.0;
    defaultEmissiveSphereMaterial.roughness = -1.0;
    defaultEmissiveSphereMaterial.anistropic = -1.0;
    defaultEmissiveSphereMaterial.sheen = -1.0;
    defaultEmissiveSphereMaterial.sheenTint = -1.0;

    

     Material defaultEmissiveSphereMaterial2 = {};
    defaultEmissiveSphereMaterial2.albedo[0] = 12.5; defaultEmissiveSphereMaterial2.albedo[1] = 12.5; defaultEmissiveSphereMaterial2.albedo[2] = 25.0; defaultEmissiveSphereMaterial2.albedo[3] = 1.0;
    defaultEmissiveSphereMaterial2.metallic = -1.0;
    defaultEmissiveSphereMaterial2.specular = -1.0;
    defaultEmissiveSphereMaterial2.specularTint = -1.0;
    defaultEmissiveSphereMaterial2.roughness = -1.0;
    defaultEmissiveSphereMaterial2.anistropic = -1.0;
    defaultEmissiveSphereMaterial2.sheen = -1.0;
    defaultEmissiveSphereMaterial2.sheenTint = -1.0;

   

                            

    
    defualtStartupScene.addObject(defaultCube);
    
    defualtStartupScene.addObject(defaultCube2);
  
    defualtStartupScene.addObject(defaultEmissiveSphere);
   
    defualtStartupScene.addObject(defaultEmissiveSphere2);
   


    defualtStartupScene.materialsOffset = defualtStartupScene.NumberOfSceneGPUMemoryBlocks - 1;
   
    defualtStartupScene.materialsInScene = (Material*)(defualtStartupScene.objectsInScene + defualtStartupScene.materialsOffset);
   
    defualtStartupScene.addMaterial(defaultCubeMaterial);
   
    defualtStartupScene.addMaterial(defaultEmissiveSphereMaterial);
   
    defualtStartupScene.addMaterial(defaultEmissiveSphereMaterial2);

    
     
 
    gameDataToRendererPushConstants* rendererPushConstantData = (gameDataToRendererPushConstants*)malloc(sizeof(gameDataToRendererPushConstants) + 24); //add 24 to allow for the footnote that input thread attaches to the cameraStore[]

     
    VkBuffer mainRendererTexturesStagingBuffer = {};
    VkDeviceMemory mainRendererTexturesStagingBufferMemory = {};
    int mainRendererTexturesStagingBufferByteSize = (4096 * 4096 * 4 * 4)*4;
    CREATE_STAGINGBUFFER(logicalGraphicsCard,physicalDeviceMemory,mainRendererTexturesStagingBufferByteSize,&mainRendererTexturesStagingBuffer,&mainRendererTexturesStagingBufferMemory);
   
    void* mainRendererTexturesStagingBufferData;
    vkMapMemory(logicalGraphicsCard,mainRendererTexturesStagingBufferMemory,0,mainRendererTexturesStagingBufferByteSize,0,&mainRendererTexturesStagingBufferData);
    exrHeaderData mainRenderExrHeader = {};

    float* mainRendererTexturesStagingBufferDataAlias = (float*)mainRendererTexturesStagingBufferData;
   
 
    readExrFile3("Image0000.exr",&mainRenderExrHeader,mainRendererTexturesStagingBufferDataAlias);

    //create render target
    VkDeviceMemory renderTargetImageMemory = {};
    VkImage renderTargetImage = {};
    VkExtent2D renderTargetImageExtent = {};
    VkImageView renderTargetImageView = {};

    VkExtent2D inputExtent = {1920,1080};
    CREATE_RENDER_TARGET_IMAGE_AND_VIEW(selectedGraphicsCard,logicalGraphicsCard,inputExtent,&physicalDeviceMemory,&renderTargetImageMemory,&renderTargetImage,&renderTargetImageExtent,&renderTargetImageView);



    

    VkImage mainRenderTextureImages = {};
    VkImageView mainRenderTextureImageViews = {};
    VkDeviceMemory mainRenderTextureImageMemory = {};
    //VkExtent2D mainRenderTextureImageExtent = {};
    VkExtent2D mainRenderTexturesImageExtent = {4096,4096};
    CREATE_TEXTURES(selectedGraphicsCard,logicalGraphicsCard,mainRenderTexturesImageExtent,3,&physicalDeviceMemory,&mainRenderTextureImageMemory,&mainRenderTextureImages,&mainRenderTextureImageViews);

   

    __atomic_store_n(&mainRenderScreenWidth,renderTargetImageExtent.width,__ATOMIC_RELAXED);
    __atomic_store_n(&mainRenderScreenHeight,renderTargetImageExtent.height,__ATOMIC_RELAXED);
    



    INITIALIZE_DESCRIPTOR_SET_MAINRENDERER(logicalGraphicsCard,&renderTargetImageView,&mainRenderTextureImageViews,&sceneDataStorageBuffer,sceneMemoryAllocationSize,descriptorSet);

//////////////////////////////////////////////////////////////////////////////////////////////////////

  
             
     
   
/////////////////////SETUP UI Compute Pipeline///////////////////////////////////////////////
    VkPipelineLayout uiComputePipelineLayout = {};
    VkPipeline uiComputePipeline = {};
    VkDescriptorSetLayout uiComputePipelineDescriptorSetLayout = {};
    CREATE_UIRENDERER_COMPUTE_PIPELINE(logicalGraphicsCard,&uiComputePipelineLayout,&uiComputePipelineDescriptorSetLayout,&uiComputePipeline);

    VkDescriptorPool uiDescriptorPool = {};
    VkDescriptorSet uiDescriptorSet = {};
    ALLOCATE_SINGLE_DESCRIPTOR_SET_SINGLE_POOL2(1,logicalGraphicsCard,&uiComputePipelineDescriptorSetLayout,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,&uiDescriptorPool,&uiDescriptorSet);

    int updateBlockBufferSize = 1000;
    int updateBlockBufferByteSize = sizeof(updateBlock) * updateBlockBufferSize;
    int uiToVulkanBufferSize = sizeof(int) + (3 * sizeof(float)) + updateBlockBufferByteSize; 

    VkBuffer updateQueueStorageBuffer = {};
    VkDeviceMemory updateQueueUniformBufferMemory = {};
    CREATE_UI_UPDATEBLOCK_QUEUE_STORAGE_BUFFER(logicalGraphicsCard,physicalDeviceMemory,uiToVulkanBufferSize,&updateQueueStorageBuffer,&updateQueueUniformBufferMemory);
 
   


    VkBuffer stagingBufferBorderTexture = {};
    VkDeviceMemory stagingBufferBorderTextureMemory = {};
    int stagingBufferBorderTextureByteSize = (120 * 1920 * 4 * 4); //changing this to 120*1920
    CREATE_STAGINGBUFFER(logicalGraphicsCard,physicalDeviceMemory,stagingBufferBorderTextureByteSize,&stagingBufferBorderTexture,&stagingBufferBorderTextureMemory);

    

    VkBuffer stagingBufferSidePaneTexture = {};
    VkDeviceMemory stagingBufferSidePaneTextureMemory = {};
    int stagingBufferSidePaneTextureByteSize = (1180 * 1920 * 4 * 4)*2; //this is overallocated needs to be fixed, the side panel texture is not nearly this wide
    CREATE_STAGINGBUFFER(logicalGraphicsCard,physicalDeviceMemory,stagingBufferSidePaneTextureByteSize,&stagingBufferSidePaneTexture,&stagingBufferSidePaneTextureMemory);
//*/

    VkBuffer stagingBufferBottomPaneTexture = {};
    VkDeviceMemory stagingBufferBottomPaneTextureMemory = {};
    int stagingBufferBottomPaneTextureByteSize = (4080 * 600 * 4 * 4) * 2;
    CREATE_STAGINGBUFFER(logicalGraphicsCard,physicalDeviceMemory,stagingBufferBottomPaneTextureByteSize,&stagingBufferBottomPaneTexture,&stagingBufferBottomPaneTextureMemory);


    VkBuffer stagingBufferGlyphAtlasTexture = {};
    VkDeviceMemory stagingBufferGlyphAtlasTextureMemory = {};
    int stagingBufferGlyphAtlasTextureByteSize = (2560*2560*4*4) * 3;
    CREATE_STAGINGBUFFER(logicalGraphicsCard,physicalDeviceMemory,stagingBufferGlyphAtlasTextureByteSize,&stagingBufferGlyphAtlasTexture,&stagingBufferGlyphAtlasTextureMemory);

 
    
    
    /*
    printf("\n\n\n\n\n\n\n\n\n crucial testing \n\n\n\n\n\n\n");
    for(int i=0; i < 1920*1080*4; i = i + 4){
        printf("stagingBufferDataAlias[%d] = %.15f,%.15f,%.15f,%.15f, \n",i,stagingBufferDataAlias[i],stagingBufferDataAlias[i+1],stagingBufferDataAlias[i+2],stagingBufferDataAlias[i+3]);
        
    }
    */



  
    //map memory for our uiToVulkan object, which contain
    void* data;
    vkMapMemory(logicalGraphicsCard,updateQueueUniformBufferMemory,0,uiToVulkanBufferSize,0,&data);
    uiToVulkanData* uiToVulkan = (uiToVulkanData*)data;
    uiData ui = uiData(1920,1080,10,200,updateBlockBufferSize,&(uiToVulkan->numberOfUpdateBlocks),uiToVulkan->updateQueue);
    ui.createPane(1,0,-1,-1,-1,-1,0,1920,0,56,0);

    int numberOfPane2MouseCollsionZones = 5;
    ui.createPane(2,0,-1,1,1,-1,0,1640,814,1080,numberOfPane2MouseCollsionZones);
    ui.createPane(3,0,1,-1,-1,-1,1640,1920,56,1080,0);  
                
    
    printf("ui.UiPanes[0].NeedsRefresh[0] = %d \n",ui.UiPanes[0].NeedsRefresh[0]);
    
   
    

  
 

    uiToVulkanPushConstantData uiPushConstantData = {};
    uiPushConstantData.testValue = 1115;
 
    uiPushConstantData.animTiming = 0.0;

    VkImage uiTextureImages[4] = {}; //0
    VkImageView uiTextureImageViews[4] = {};
    VkDeviceMemory uiTextureImageMemory[4] = {};
    


    VkExtent2D uiBorderTextureExtent = {120,1920};
    CREATE_TEXTURES(selectedGraphicsCard,logicalGraphicsCard,uiBorderTextureExtent,1,&physicalDeviceMemory,uiTextureImageMemory,uiTextureImages,uiTextureImageViews);
    

    VkExtent2D uiSidePanelTextureExtent = {1180,1920};
    CREATE_TEXTURES(selectedGraphicsCard,logicalGraphicsCard,uiSidePanelTextureExtent,10,&physicalDeviceMemory,uiTextureImageMemory+1,uiTextureImages+1,uiTextureImageViews+1);
    
    VkExtent2D uiBottomPanelTextureExtent = {4080,600};
    CREATE_TEXTURES(selectedGraphicsCard,logicalGraphicsCard,uiBottomPanelTextureExtent,10,&physicalDeviceMemory,uiTextureImageMemory+2,uiTextureImages+2,uiTextureImageViews+2);
    

    VkExtent2D uiGlyphAtlasTextureExtent = {2560,2560};
    CREATE_TEXTURES(selectedGraphicsCard,logicalGraphicsCard,uiGlyphAtlasTextureExtent,3,&physicalDeviceMemory,uiTextureImageMemory+3,uiTextureImages+3,uiTextureImageViews+3);
    
    

    VkExtent2D finalCompositeImageExtent = {1920,1080};
    VkImage finalCompositeImage = {};
    VkImageView finalCompositeImageView = {};
    VkDeviceMemory finalCompositeImageMemory = {};
    CREATE_FINALCOMPOSITE_IMAGE(selectedGraphicsCard,logicalGraphicsCard,finalCompositeImageExtent,&physicalDeviceMemory,&finalCompositeImageMemory,&finalCompositeImage,&finalCompositeImageView);

    

    INITIALIZE_DESCRIPTOR_SET_UIRENDERER(logicalGraphicsCard,&finalCompositeImageView,uiTextureImageViews,uiToVulkanBufferSize,&updateQueueStorageBuffer,uiDescriptorSet);
   
   

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


     

     
    



    VkCommandPool commandPool = {};
    CREATE_COMMAND_POOL(logicalGraphicsCard,queueFamilyIndices[0],&commandPool);



    VkCommandBuffer commandBuffer = {};
    CREATE_COMMAND_BUFFER(logicalGraphicsCard,commandPool,&commandBuffer);




    VkSemaphore imageAvailableSemaphores[swapChainImageCount] = {};
    VkSemaphore renderFinishedSemaphores[swapChainImageCount] = {};
    VkFence inFlightFence = {};
    CREATE_SYNC_OBJECTS(logicalGraphicsCard,swapChainImageCount,imageAvailableSemaphores,renderFinishedSemaphores,&inFlightFence);


    


    VkImage renderTargetImages[2] = {renderTargetImage, finalCompositeImage};
    VkExtent2D renderTargetImageExtents[2] = {renderTargetImageExtent,finalCompositeImageExtent};
    VkPipeline computePipelines[2] = {computePipeline,uiComputePipeline};
    VkPipelineLayout computePipelineLayouts[2] = {computePipelineLayout, uiComputePipelineLayout};
    VkDescriptorSet descriptorSets[2] = {descriptorSet, uiDescriptorSet};

   


  
    INITIALIZE_IMAGE_LAYOUTS(logicalGraphicsCard,computeQueue,&commandBuffer,uiTextureImages,&mainRenderTextureImages,renderTargetImages,swapChainImageCount,swapChainImages,&inFlightFence);

    
    
     


   

    //load mainRender Textures
    loadTexturesMainRender(logicalGraphicsCard,computeQueue,&commandBuffer,&mainRenderTextureImages,&mainRendererTexturesStagingBuffer,&inFlightFence,mainRenderTexturesImageExtent,0,3);
    
    
    



   



    //Map memory for border texture staging buffer
    void* stagingBufferDataBorderTexture;
    vkMapMemory(logicalGraphicsCard,stagingBufferBorderTextureMemory,0,stagingBufferBorderTextureByteSize,0,&stagingBufferDataBorderTexture);
    exrHeaderData borderTextureExrFileHeader = {};
    float* stagingBufferDataBorderGraphicAlias = (float*)stagingBufferDataBorderTexture;
    


    //I should ensure that the '/00' does not need to be changed to a /0 once filenames get above 100
    //Build filenames for side pane textures and map memory for side pane texture staging buffer
    char* sidePaneTextureFileNames[40];
    const char* sidePaneTextureFileNameBase = "sidePaneAnim/00";
    const char* sidePaneTextureFileNameExtensionTypeName = ".exr";
    char sidePaneTextureFileNameNumberExtension[4];
    for(int i = 0 ; i < 40; i++){
        integerToString(i+70,sidePaneTextureFileNameNumberExtension,1);
        char* sidePaneFileName = concatenateStrings2(sidePaneTextureFileNameBase,sidePaneTextureFileNameNumberExtension,sidePaneTextureFileNameExtensionTypeName);
        sidePaneTextureFileNames[i] = sidePaneFileName;

        
    }    
    void* stagingBufferDataSidePaneTexture;
    vkMapMemory(logicalGraphicsCard,stagingBufferSidePaneTextureMemory,0,stagingBufferSidePaneTextureByteSize,0,&stagingBufferDataSidePaneTexture);
    exrHeaderData sidePaneTextureExrFileHeader = {};
    float* stagingBufferDataAliasSidePane = (float*)stagingBufferDataSidePaneTexture;

    


    //Build filenames for bottom pane textures and map memory for bottom pane texture staging buffer
    char* bottomPaneTextureFileNames[40];
    const char* bottomPaneTextureFileNameBase = "bottomPaneAnim/00";
    const char* bottomPaneTextureFileNameExtensionTypeName = ".exr";
    char bottomPaneTextureFileNameNumberExtension[4];
    for(int i = 0 ; i < 40; i++){
        integerToString(i+10,bottomPaneTextureFileNameNumberExtension,1);
        char* bottomPaneFilename = concatenateStrings2(bottomPaneTextureFileNameBase,bottomPaneTextureFileNameNumberExtension,bottomPaneTextureFileNameExtensionTypeName);
        bottomPaneTextureFileNames[i] = bottomPaneFilename;

        
    }    
    void* stagingBufferDataBottomPaneTexture;
    vkMapMemory(logicalGraphicsCard,stagingBufferBottomPaneTextureMemory,0,stagingBufferBottomPaneTextureByteSize,0,&stagingBufferDataBottomPaneTexture);
    exrHeaderData bottomPaneTextureExrFileHeader = {};
    float* stagingBufferDataAliasBottomPane = (float*)stagingBufferDataBottomPaneTexture;


    //map memory for glyph atlas 
    void* stagingBufferDataGlyphAtlasTexture;
    vkMapMemory(logicalGraphicsCard,stagingBufferGlyphAtlasTextureMemory,0,stagingBufferGlyphAtlasTextureByteSize,0,&stagingBufferDataGlyphAtlasTexture);
    exrHeaderData glyphAtlasTextureExrFileHeader;
    float* stagingBufferDataAliasGlyphAtlasTexture = (float*)stagingBufferDataGlyphAtlasTexture;



    
    

    
    

    //Load in the Border UI Texture
    readExrFile3("borderTexture.exr",&borderTextureExrFileHeader,stagingBufferDataBorderGraphicAlias);
    loadTexturesUI(logicalGraphicsCard,computeQueue,&commandBuffer,uiTextureImages,&stagingBufferBorderTexture,&inFlightFence,uiBorderTextureExtent,0,1);

    //Load in the Side Panel UI Textures
    for(int i = 0; i < 10; i = i + 2){
        readExrFile3(sidePaneTextureFileNames[i],&sidePaneTextureExrFileHeader,stagingBufferDataAliasSidePane);
        readExrFile3(sidePaneTextureFileNames[i+1],&sidePaneTextureExrFileHeader,stagingBufferDataAliasSidePane + (1180 * 1920 * 4));
        int baseLayer = i;
        int layerCount = 2;
        loadTexturesUI(logicalGraphicsCard,computeQueue,&commandBuffer,uiTextureImages+1,&stagingBufferSidePaneTexture,&inFlightFence,uiSidePanelTextureExtent,baseLayer,layerCount);

    }


    //Load in the BottomPanel UI Textures
    for(int i = 0; i < 10; i = i + 2){
        readExrFile3(bottomPaneTextureFileNames[i],&bottomPaneTextureExrFileHeader,stagingBufferDataAliasBottomPane);
        readExrFile3(bottomPaneTextureFileNames[i+1],&bottomPaneTextureExrFileHeader,stagingBufferDataAliasBottomPane + (4080 * 600 * 4));
        int baseLayer = i;
        int layerCount = 2;
        loadTexturesUI(logicalGraphicsCard,computeQueue,&commandBuffer,uiTextureImages+2,&stagingBufferBottomPaneTexture,&inFlightFence,uiBottomPanelTextureExtent,baseLayer,layerCount);

    }


    //Load in the Glyph Atlas UI Textures

    readExrFile3("GlyphAtlas0.exr",&glyphAtlasTextureExrFileHeader,stagingBufferDataAliasGlyphAtlasTexture + (2560 * 2560 * 4 *4));
    readExrFile3("GlyphAtlas1.exr",&glyphAtlasTextureExrFileHeader,stagingBufferDataAliasGlyphAtlasTexture + (2560 * 2560 * 4 *4) * 2);
    readExrFile3("GlyphAtlas2.exr",&glyphAtlasTextureExrFileHeader,stagingBufferDataAliasGlyphAtlasTexture + (2560 * 2560 * 4 *4) * 3);
    loadTexturesUI(logicalGraphicsCard,computeQueue,&commandBuffer,uiTextureImages+3,&stagingBufferGlyphAtlasTexture,&inFlightFence,uiGlyphAtlasTextureExtent,0,3);
   
   
    

    


    
   
    initialize_CameraData(&(rendererPushConstantData->mainCamera),renderTargetImageExtent.width,renderTargetImageExtent.height);
///*
    cameraData* cameraStores = (cameraData*)malloc((24 + sizeof(cameraData)) * 4); //the 24 extra byrs is for the footnote used by input thread, the header is included on the cameraData object itself
   
    for(int i = 0; i < 4; i++){
        initialize_CameraData(cameraStores+i,renderTargetImageExtent.width,renderTargetImageExtent.height);
    }     
//*/

    
    
      
  
    //mouse updates are handled by the ui on the main thread, unlike camera updates which are handled by the input thread
    inputThreadInputData inputThreadData = {cameraStores,&ui};

    char threadStack1[9000];
    int childPID = clone(backgroundThread,threadStack1+9000,CLONE_FILES | CLONE_FS | CLONE_IO | CLONE_VM, NULL);

    //5mb memory for input thread
    char threadStack2[5242880];
    int childPID2 = clone(inputThread,threadStack2+5242880,CLONE_FILES | CLONE_FS | CLONE_IO | CLONE_VM, &inputThreadData);

    bool inputThreadIsReady = 0;
    while(!inputThreadIsReady){
        __atomic_load(&inputThreadReady,&inputThreadIsReady,__ATOMIC_RELAXED);
    }
  
    //fps init///////////////////////////////
    double lastTime = getCurrentTime();
    double timer = lastTime;
    int frameCount = 0;
    double fps = 0;
   /////////////////////////////////////////
 
    int semaphoreIndex = 0;

    

    bool inputThread_To_MainThread_SynchronizationFailure_HARDCRASH = 0;
    int inputMode = 1;
    bool storeIndexMain = 0;   
    uint64_t storeReadyEpoch = 0; 

    
    
    bool skipInputThisFrame = 0;
    
    bool swapChainImageFirstPass[3] = {1,1,1};

    double animTime = 0.0;
    double animTime2 = 0.0;
    double animFrameTimeLength = 2.0;
    double animFrameTimeLength2 = 60.0;
    int numberOfFramesInAnim = 10;
    double animDuration = animFrameTimeLength * numberOfFramesInAnim;
    int currentAnimFrame = 0;
    
    uint64_t numberOfSkippedInputs = 0;

    while(running){                         
                 
        
         
        //fps calc///////////////////////////////
        double currentTime = getCurrentTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        //////////////////////////////////////////
        //printf("MAIN THREAD: input mode1 = %d \n",inputMode);
          
        animTime = animTime + deltaTime;
        animTime2 = animTime2 + deltaTime;

        if (animTime > animFrameTimeLength) {
            animTime = 0.0; // clamp to the duration
            currentAnimFrame = currentAnimFrame + 1;
            if(currentAnimFrame > 8){
                currentAnimFrame = 0;
            }
       
        }

        if(animTime2 > animFrameTimeLength2){
            animTime2 = 0.0;
        }                

         // Calculate the normalized progress (0.0 to 1.0)
        float animTiming = animTime / animFrameTimeLength;
        float animTiming2 = animTime2 / animFrameTimeLength2;
        //printf("animTiming = %.15f \n", animTiming);
        uiPushConstantData.currentFrame = currentAnimFrame;
        uiPushConstantData.animTiming = animTiming;
        uiPushConstantData.animTiming2 = animTiming2;
        //printf("sidePanelVRangeStart = %.15f \n", uiPushConstantData.sidePanelVRangeStart);
        //printf("sidePanelVRangeEnd = %.15f \n", uiPushConstantData.sidePanelVRangeEnd);
    
        //printf("running %d \n",running);
        
        



 

         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         ///////////////////////////////////////////////////            -SUBSEQUENT ITERATIONS-      /////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////ENSURE NO MISSED PUBLICATION IN OTHER SLOT/////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////              CLEAR THE SLOTS              /////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////       FLIP THE LOCAL 'storeIndexMain'     /////////////////////////////////////////////////////////////////
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(frameCount > 0){
            //first check whether last frames "mainSwitch" was acknowledged by the input thread
            //Then check the storeReadyValue of the "Other Slot", and ensure we dont need to reconcile
            //if we havent acknowledged then we should be able to assure that epoch_Counter <= epoch_Counters + 1
           
            
            if(isMainSwitchAcknowledged(!storeIndexMain) == true){skipInputThisFrame=0;}
            else{skipInputThisFrame = 1; numberOfSkippedInputs = numberOfSkippedInputs + 1;}//Then this is the path where the input thread has NOT achnowledged the switch, and thus we should SKIP trying to read new input this frame
            
            
            int otherSlotStoreReadyValue;
            __atomic_load(data_ready+((2*storeIndexMain) + !(storeReadyEpoch%2)),&otherSlotStoreReadyValue,__ATOMIC_RELAXED);

            if((otherSlotStoreReadyValue == -1) && (skipInputThisFrame == 0)){
                
                //If the main switch has been acknowledged (as insinuated by skipInputThisFrame==0), Then the occurence of a -1 data ready value in one of the slots owned by the main thread Necessitates a hard crash - as this should never happen
                inputThread_To_MainThread_SynchronizationFailure_HARDCRASH = 1;
                __atomic_store_n(&running,0,__ATOMIC_RELAXED);
                break;                  
            }
            else if(otherSlotStoreReadyValue == 1){
                //by checking the epoch counters, we validate that the other slot doesnt contain a missed publication
                //if it does contain a missed publication we reconcile with it 
                uint64_t epochCounterSlotReads[2];
                __atomic_load(epoch_Counters+((2*storeIndexMain) + (storeReadyEpoch%2)),epochCounterSlotReads,__ATOMIC_RELAXED);
                __atomic_load(epoch_Counters+((2*storeIndexMain) + !(storeReadyEpoch%2)),epochCounterSlotReads+1,__ATOMIC_RELAXED);

                //check to see if we need to reconcile
                if(epochCounterSlotReads[0] < epochCounterSlotReads[1]){
                    //reconcile
                }
            }



            __atomic_store_n(data_ready+(2*storeIndexMain), 0, __ATOMIC_RELAXED);
            __atomic_store_n(data_ready+(2*storeIndexMain) + 1, 0, __ATOMIC_RELAXED);

            storeIndexMain = !storeIndexMain;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
so if no input is received the first frame will acquire 0 as store_ready_epoch and perform the switch
the second iteration should then
*/

 
        

    if(skipInputThisFrame == 0){
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////PERFORM THE MAIN SWITCH////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __atomic_load(&store_Ready_Epoch,&storeReadyEpoch,__ATOMIC_ACQUIRE);
        __atomic_store_n(&store_Index_Main,!storeIndexMain,__ATOMIC_RELAXED);
        //if((otherSlotStoreReadyValue == -1) && (storeReadyEpoch != previousFrameStoreReadyIndex)){} 
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////READ FROM ACQUIRED SLOT///////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //to ensure the read is safe, the payload contains an encoded header and footnote that decodes to the inputThreadEpoch that published it
        //that number should match with the storeReadyEpoch that we acquired and the epochCounterSlot

        //if it doesnt then it means the input thread has "overspinned" during/after the main switch (an overspin means that epoch the input thread is currently observing is 2+ higher than the acquired storeReadyEpoch)
        //If the second attempt fails, then we perform a RMW operation with __atomic_exchange() this locks and invalidates other core's cachelines contianing this variable, explictly ensuring that another core loading this variable after the exchange gets the current updated value
        __atomic_load(&input_mode,&inputMode,__ATOMIC_RELAXED);
        
        int stateOfReading = -1;
        
        for(int i = 0; i < 2; i++){
            
          
            if(i==1){
                __atomic_exchange_n(&store_Index_Main,!storeIndexMain,__ATOMIC_RELAXED);
               
            }//atomic exchange
            


            
    


            int storeIndex = 2 * storeIndexMain + (storeReadyEpoch%2);
            uint64_t epochCounterSlotRead;
            __atomic_load(&epoch_Counters[storeIndex], &epochCounterSlotRead,__ATOMIC_RELAXED);
            int dataReadyReading = data_ready[storeIndex];
            if( (dataReadyReading == 1)){
        
                if(inputMode == 0){
                    //printf("MAIN THREAD: inputmode = %d \n",inputMode);
                    //remember the first 24 bytes of cameraData arent needed and were used to store the inputs header verification
                    cameraData* cameraStorePointer = (cameraData*)(((char*)cameraStores) + (storeIndex * (sizeof(cameraData) + 24)));
                    memcpy(&(rendererPushConstantData->mainCamera),cameraStorePointer,sizeof(cameraData) + 24);
                    uint64_t* pointerToFootnote = (  (uint64_t*) ((char*)(&(rendererPushConstantData->mainCamera)) + sizeof(cameraData))  );
                    uint64_t* pointerToHeader = ((uint64_t*)(&(rendererPushConstantData->mainCamera)));

                    if(verifyMarker(pointerToHeader,storeReadyEpoch) && verifyMarker(pointerToFootnote,storeReadyEpoch)){
                        stateOfReading = 1;
                    }
                }

                if(inputMode == 1){
                    
                    ui.uiMouse.mouseXY = ui.uiMouse.mouseXY_Stores[storeIndex];
                    
                    ui.uiMouse.mouseKeys = ui.uiMouse.mouseKeys_Stores[storeIndex];
                    

                    int numberOfEntriesInPass = ui.KeyPressRecordStores[(storeIndex * ui.KeyPressRecordBufferSize) + 1].paddingForInputHeader;
                    memcpy(ui.KeyPressRecordList, ui.KeyPressRecordStores + (storeIndex * ui.KeyPressRecordBufferSize), sizeof(KeyPressRecord) * (numberOfEntriesInPass + 3));
                   

                    bool allMarkerCorrect = verifyMarker(((uint64_t*)&(ui.uiMouse.mouseXY.InputSystemHeaderMarkers)), storeReadyEpoch) && verifyMarker(((uint64_t*)&(ui.uiMouse.mouseXY.InputSystemFootnoteMarkers)), storeReadyEpoch) &&
                                            verifyMarker(ui.uiMouse.mouseKeys.InputSystemHeaderMarkers, storeReadyEpoch) && verifyMarker(ui.uiMouse.mouseKeys.InputSystemFootnoteMarkers, storeReadyEpoch) &&
                                            verifyMarker((uint64_t*)(ui.KeyPressRecordList), storeReadyEpoch) && verifyMarker((uint64_t*)(ui.KeyPressRecordList + numberOfEntriesInPass + 2),storeReadyEpoch);
                    
                    if(allMarkerCorrect == 1){printf("ALL MARKERS CORRECT \n"); stateOfReading =1; break;}
                }


            }
            else if(dataReadyReading == 0){
                stateOfReading = 0;
                break;
            }
            else if(epochCounterSlotRead != storeReadyEpoch){
                stateOfReading = 0;
                printf("OVERSPIN DETECTED: epochCounterSlotRead = %d : storeReadyEpoch = %d \n", epochCounterSlotRead, storeReadyEpoch);
            }
            if(stateOfReading == -1){ //Then either the markers in the payload != storeReadyEpoch, dataReadyReading == -1, or epochCountersSlotRead != storeReadyEpoch, in any of these cases we try to read the other slot
                stateOfReading = 0;
                printf("VERIFICATION OF READ FAILED \n");
            }


                                
        

 
        }

        if(stateOfReading == -1){
            printf("VERIFICATION OF READ FAILED 2 \n");
            running = 0; 
            
            break;} //crash
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        }
 
       

        

   
        printf("ABOUT TO DEBUG PROCESS UPDATES \n");

        defualtStartupScene.prepareToRenderScene();
        //printf("THIS IS WHERE THE BUG IS ITS AT prepareTORenderScene() \n");

        ui.processUpdates();         
        printf("uiToVulkan->numberOfUpdateBlocks = %d \n",uiToVulkan->numberOfUpdateBlocks);
        
                  
         
        for(int i = 0; i < uiToVulkan->numberOfUpdateBlocks;i++){
            printf("uiToVulkan->updateQueue[%d].ID = %d \n",i,uiToVulkan->updateQueue[i].ID);
            printf("uiToVulkan->updateQueue[%d].pixelIndex = %d \n",i,uiToVulkan->updateQueue[i].pixelIndex);
            printf("uiToVulkan->updateQueue[%d].width = %d \n",i,uiToVulkan->updateQueue[i].width);
            printf("uiToVulkan->updateQueue[%d].height = %d \n",i,uiToVulkan->updateQueue[i].height);
            printf("uiToVulkan->updateQueue[%d].topLeftPixelCoordinateX = %d \n",i,uiToVulkan->updateQueue[i].topLeftPixelCoordinateX);
            printf("uiToVulkan->updateQueue[%d].topLeftPixelCoordinateY = %d \n",i,uiToVulkan->updateQueue[i].topLeftPixelCoordinateY); 
            printf("uiToVulkan->updateQueue[%d].Layer = %d \n",i,uiToVulkan->updateQueue[i].Layer);
  
        } 
        printf("ui.NumberOfDispatches = %d \n",ui.NumberOfDispatches);
        for(int i = 0; i < ui.NumberOfDispatches; i++){
            printf("ui.TotalNumberOfPixelsInDispatch[%d] = %d \n",i,ui.TotalNumberOfPixelsInDispatch[i]);
            printf("ui.DispatchStartIndices[%d] = %d \n",i,ui.DispatchStartIndices[i]);
            printf("ui.DispatchLastIndices[%d] = %d \n",i,ui.DispatchLastIndices[i]);
        }
        
        
        rendererPushConstantData->totalNumberOfObjects = defualtStartupScene.numberOfObjects;
        rendererPushConstantData->numberOfNonEmissiveObjects = defualtStartupScene.numberOfNonEmissiveObjects;
        rendererPushConstantData->numberOfEmissiveObjects = defualtStartupScene.numberOfEmissiveObjects;
        rendererPushConstantData->materialsOffset = defualtStartupScene.materialsOffset;
        DRAW_FRAME(logicalGraphicsCard,computeQueue,presentationQueue,swapChain,&commandBuffer,computePipelines,computePipelineLayouts,descriptorSets,&inFlightFence,imageAvailableSemaphores,renderFinishedSemaphores,semaphoreIndex,swapChainImageFirstPass,swapChainImages,renderTargetImages,swapChainExtent,renderTargetImageExtents,updateQueueStorageBuffer,&ui,rendererPushConstantData,&uiPushConstantData);
        //clear ui update queue 
        //printf("FINISHED DRAW FRAME \n");  
         
 
          
        //fps calc///////////////////////////////
        frameCount++;

        if (currentTime - timer > 1.0) {
            fps = frameCount / (currentTime - timer);
            printf("FPS: %.2f\n \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", fps);

            timer += 1.0;
            frameCount = 0;
        }
        //////////////////////////////////////////
 
        if(running){
            *(ui.UpdateBlockQueueIndex)  = 0;
        }

                 
   
        semaphoreIndex = (semaphoreIndex + 1) % 3;
    }          
    
   
    printf("numberOfSkippedInputs = %d \n",numberOfSkippedInputs);
   
         
    vkDeviceWaitIdle(logicalGraphicsCard);

    kill(childPID2,SIGUSR1);      

    int waitStatus1;
    waitpid(childPID2,&waitStatus1,0);
    int waitStatus2; 
    waitpid(childPID,&waitStatus2,0);

     
    bool isInputThreadShutDown = false;
    __atomic_load(&inputThreadShutDown, &isInputThreadShutDown, __ATOMIC_RELAXED);
    while(!inputThreadShutDown){
        printf("STUCK HERE \n");
        __atomic_load(&inputThreadShutDown, &isInputThreadShutDown, __ATOMIC_RELAXED);
        //sleep(1);
    }
    printf("shutdown free memory of bounding boxes \n");
    for(int nn = 0; nn < ui.UiPanes[1].NumberOfFreeBoundingBoxes; nn++){
        ui.UiPanes[1].FreeBoundingBoxes[nn].freeMemory();
    }                                 



    
    printf("Last Line before cleanup \n");
    CLEANUP(&xcbWindow,instance, logicalGraphicsCard, surface,swapChain,swapChainImageCount,computePipelineLayout,computePipeline,commandPool,commandBuffer,imageAvailableSemaphores,renderFinishedSemaphores,inFlightFence,descriptorPool,computePipelineDescriptorSetLayout,renderTargetImage,renderTargetImageMemory,renderTargetImageView,debugMessenger);

    printf("Last Line \n");


    
    return 0;                   
               
}    
