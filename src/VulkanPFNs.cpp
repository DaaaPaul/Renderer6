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
    void fSetInstance(VkInstance const& givenInstance) {
        gInstanceInUse = givenInstance;
    }

    void fLoadVkGetInstanceProcAddr() {
        #ifdef _WIN32
        std::cout << "You are running with windows. Loading the vulkan loader...\n";

        HMODULE pVulkanLoader = LoadLibraryA("vulkan-1.dll");
        CHECK_NULLPTR(pVulkanLoader)

        gpVkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(pVulkanLoader, "vkGetInstanceProcAddr"));
        CHECK_NULLPTR(gpVkGetInstanceProcAddr)
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
        CHECK_NULLPTR(pVulkanLoader) // if nothing was found, throw std::runtime_error

        gpVkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(pVulkanLoader, "vkGetInstanceProcAddr"));
        CHECK_NULLPTR(gpVkGetInstanceProcAddr)
        #endif
    }

    void fLoadTrueGlobalVulkanFunctions() {
        std::cout << "Loading vulkan true global functions...\n";

        gpVkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(gpVkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties"));
        CHECK_NULLPTR(gpVkEnumerateInstanceExtensionProperties)
        gpVkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(gpVkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties"));
        CHECK_NULLPTR(gpVkEnumerateInstanceLayerProperties)
        gpVkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(gpVkGetInstanceProcAddr(nullptr, "vkCreateInstance"));
        CHECK_NULLPTR(gpVkCreateInstance)
    }

    void fLoadVulkanFunctions() {
        CHECK_NULLPTR(gInstanceInUse);

        std::cout << "Loading vulkan functions...\n";

        gpVkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkDestroyInstance"));
        CHECK_NULLPTR(gpVkDestroyInstance)

        gpVkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkDestroySurfaceKHR"));
        CHECK_NULLPTR(gpVkDestroySurfaceKHR)

        gpVkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkEnumeratePhysicalDevices"));
        CHECK_NULLPTR(gpVkEnumeratePhysicalDevices)

        gpVkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetPhysicalDeviceProperties"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceProperties)

        gpVkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetPhysicalDeviceQueueFamilyProperties"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceQueueFamilyProperties)

        gpVkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkEnumerateDeviceExtensionProperties"));
        CHECK_NULLPTR(gpVkEnumerateDeviceExtensionProperties)

        gpVkGetPhysicalDeviceFeatures2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkGetPhysicalDeviceFeatures2"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceFeatures2)

        gpVkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkCreateDevice"));
        CHECK_NULLPTR(gpVkCreateDevice)

        gpVkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(gpVkGetInstanceProcAddr(gInstanceInUse, "vkDestroyDevice"));
        CHECK_NULLPTR(gpVkDestroyDevice)
    }
}