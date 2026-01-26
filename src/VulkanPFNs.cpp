#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <dlfcn.h>
#endif
#include <iostream>
#include "Common.h"
#include "VulkanPFNs.h"

void setInstance(VkInstance const& givenInstance) {
    instanceInUse = givenInstance;
}

void loadVkGetInstanceProcAddr() {
    #ifdef _WIN32
    std::cout << "You are running with windows. Loading the vulkan loader...\n";

    HMODULE pVulkanLoader = LoadLibraryA("vulkan-1.dll");
    CHECK_NULLPTR(pVulkanLoader)

    pVkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(pVulkanLoader, "vkGetInstanceProcAddr"));
    CHECK_NULLPTR(pVkGetInstanceProcAddr)
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

	pVkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(pVulkanLoader, "vkGetInstanceProcAddr"));
    CHECK_NULLPTR(pVkGetInstanceProcAddr)
    #endif
}

void loadTrueGlobalVulkanFunctions() {
    std::cout << "Loading vulkan true global functions...\n";

    pVkEnumerateInstanceExtensionProperties = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(pVkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties"));
    CHECK_NULLPTR(pVkEnumerateInstanceExtensionProperties)
    pVkEnumerateInstanceLayerProperties = reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(pVkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties"));
    CHECK_NULLPTR(pVkEnumerateInstanceLayerProperties)
    pVkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(pVkGetInstanceProcAddr(nullptr, "vkCreateInstance"));
    CHECK_NULLPTR(pVkCreateInstance)
    pVkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(pVkGetInstanceProcAddr(nullptr, "vkDestroyInstance"));
    CHECK_NULLPTR(pVkDestroyInstance)
}

void loadVulkanFunctions() {
    std::cout << "Loading vulkan functions...\n";
}