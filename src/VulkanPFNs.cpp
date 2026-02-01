#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#ifdef __APPLE__
#include <dlfcn.h>
#endif
#include <iostream>
#include "Common.h"
#include "VulkanPFNs.h"

namespace VulkanPFNs {
    void fLoadVkGetInstanceProcAddr() {
        #ifdef _WIN32
        std::cout << "You are running with windows. Loading the vulkan loader...\n";

        HMODULE pVulkanLoader = LoadLibraryA("vulkan-1.dll");
        CHECK_NULLPTR(pVulkanLoader, "Failed to dynamically load vulkan-1.dll")

        gpVkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(pVulkanLoader, "vkGetInstanceProcAddr"));
        CHECK_NULLPTR(gpVkGetInstanceProcAddr, "Failed to find vkGetInstanceProcAddr in vulkan-1.dll")
        #endif

        #ifdef __APPLE__
        std::cout << "You are running with mac. Loading the vulkan loader...\n";

        void* pVulkanLoader = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);

        // fallback search if libvulkan.dylib is not found
        if (!pVulkanLoader) {
            pVulkanLoader = dlopen("/opt/homebrew/lib/libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
        } 
        if (!pVulkanLoader) {
            pVulkanLoader = dlopen("libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL);
        } 
        if (!pVulkanLoader && getenv("DYLD_FALLBACK_LIBRARY_PATH") == NULL) {
            pVulkanLoader = dlopen("/usr/local/lib/libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
        }
        if (!pVulkanLoader) {
            pVulkanLoader = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
        }
        if (!pVulkanLoader) {
            pVulkanLoader = dlopen("vulkan.framework/vulkan", RTLD_NOW | RTLD_LOCAL);
        }
        if (!pVulkanLoader) {
            pVulkanLoader = dlopen("MoltenVK.framework/MoltenVK", RTLD_NOW | RTLD_LOCAL);
        }
        CHECK_NULLPTR(pVulkanLoader, "Failed to dynamically load the vulkan loader on your mac") // if nothing was found, throw std::runtime_error

        gpVkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(pVulkanLoader, "vkGetInstanceProcAddr"));
        CHECK_NULLPTR(gpVkGetInstanceProcAddr, "Failed to find vkGetInstanceProcAddr on the vulkan loader on your mac")
        #endif
    }

    void fLoadTrueGlobalVulkanFunctions() {
        std::cout << "Loading vulkan true global functions...\n";

        gpVkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(gpVkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties"));
        CHECK_NULLPTR(gpVkEnumerateInstanceExtensionProperties, "Failed to load vkEnumerateInstanceExtensionProperties")
        gpVkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(gpVkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties"));
        CHECK_NULLPTR(gpVkEnumerateInstanceLayerProperties, "Failed to load vkEnumerateInstanceLayerProperties")
        gpVkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(gpVkGetInstanceProcAddr(nullptr, "vkCreateInstance"));
        CHECK_NULLPTR(gpVkCreateInstance, "Failed to load vkCreateInstance")
    }

    void fLoadVulkanFunctions() {
        CHECK_NULLPTR(gInstanceInUse, "There is no instance in use for the VulkanPFNs namespace bro");

        std::cout << "Loading vulkan functions...\n";

        gpVkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkDestroyInstance"));
        CHECK_NULLPTR(gpVkDestroyInstance, "Failed to load vkDestroyInstance")

        gpVkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkDestroySurfaceKHR"));
        CHECK_NULLPTR(gpVkDestroySurfaceKHR, "Failed to load vkDestroySurfaceKHR")

        gpVkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkEnumeratePhysicalDevices"));
        CHECK_NULLPTR(gpVkEnumeratePhysicalDevices, "Failed to load vkEnumeratePhysicalDevices")

        gpVkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetPhysicalDeviceProperties"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceProperties, "Failed to load vkGetPhysicalDeviceProperties")

        gpVkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetPhysicalDeviceQueueFamilyProperties"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceQueueFamilyProperties, "Failed to load vkGetPhysicalDeviceQueueFamilyProperties")

        gpVkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkEnumerateDeviceExtensionProperties"));
        CHECK_NULLPTR(gpVkEnumerateDeviceExtensionProperties, "Failed to load vkEnumerateDeviceExtensionProperties")

        gpVkGetPhysicalDeviceFeatures2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetPhysicalDeviceFeatures2"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceFeatures2, "Failed to load vkGetPhysicalDeviceFeatures2")

        gpVkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkCreateDevice"));
        CHECK_NULLPTR(gpVkCreateDevice, "Failed to load vkCreateDevice")

		gpVkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetDeviceQueue"));
		CHECK_NULLPTR(gpVkGetDeviceQueue, "Failed to load gpVkGetDeviceQueue")

        gpVkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkDestroyDevice"));
        CHECK_NULLPTR(gpVkDestroyDevice, "Failed to load vkDestroyDevice")

        gpVkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceSurfaceCapabilitiesKHR, "Failed to load vkGetPhysicalDeviceSurfaceCapabilitiesKHR")

        gpVkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceSurfaceFormatsKHR, "Failed to load vkGetPhysicalDeviceSurfaceFormatsKHR")

        gpVkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceSurfacePresentModesKHR, "Failed to load vkGetPhysicalDeviceSurfacePresentModesKHR")

        gpVkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkCreateSwapchainKHR"));
        CHECK_NULLPTR(gpVkCreateSwapchainKHR, "Failed to load vkCreateSwapchainKHR")

        gpVkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkDestroySwapchainKHR"));
        CHECK_NULLPTR(gpVkDestroySwapchainKHR, "Failed to load vkDestroySwapchainKHR")
        
        gpVkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetSwapchainImagesKHR"));
        CHECK_NULLPTR(gpVkGetSwapchainImagesKHR, "Failed to load gpVkGetSwapchainImagesKHR")

		gpVkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkCreateImageView"));
		CHECK_NULLPTR(gpVkCreateImageView, "Failed to load gpVkCreateImageView")
			
		gpVkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkDestroyImageView"));
		CHECK_NULLPTR(gpVkDestroyImageView, "Failed to load gpVkDestroyImageView")

		gpVkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkCreateBuffer"));
		CHECK_NULLPTR(gpVkCreateBuffer, "Failed to load gpVkCreateBuffer")

		gpVkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkDestroyBuffer"));
		CHECK_NULLPTR(gpVkDestroyBuffer, "Failed to load gpVkDestroyBuffer")
			
		gpVkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetPhysicalDeviceMemoryProperties"));
		CHECK_NULLPTR(gpVkGetPhysicalDeviceMemoryProperties, "Failed to load gpVkGetPhysicalDeviceMemoryProperties")

		gpVkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetBufferMemoryRequirements"));
		CHECK_NULLPTR(gpVkGetBufferMemoryRequirements, "Failed to load gpVkGetBufferMemoryRequirements")
			
		gpVkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkAllocateMemory"));
		CHECK_NULLPTR(gpVkAllocateMemory, "Failed to load gpVkAllocateMemory")

		gpVkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkBindBufferMemory"));
		CHECK_NULLPTR(gpVkBindBufferMemory, "Failed to load gpVkBindBufferMemory")

		gpVkMapMemory = reinterpret_cast<PFN_vkMapMemory>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkMapMemory"));
		CHECK_NULLPTR(gpVkMapMemory, "Failed to load gpVkMapMemory")

		gpVkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkUnmapMemory"));
		CHECK_NULLPTR(gpVkUnmapMemory, "Failed to load gpVkUnmapMemory")

		gpVkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkFreeMemory"));
		CHECK_NULLPTR(gpVkFreeMemory, "Failed to load gpVkFreeMemory")
    }
}