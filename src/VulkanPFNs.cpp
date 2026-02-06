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
        CHECK_NULLPTR(gpInstanceUsed, "There is no instance in use for the VulkanPFNs namespace bro");

        std::cout << "Loading vulkan functions...\n";

        gpVkDestroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroyInstance"));
        CHECK_NULLPTR(gpVkDestroyInstance, "Failed to load vkDestroyInstance")

        gpVkDestroySurfaceKHR = reinterpret_cast<PFN_vkDestroySurfaceKHR>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroySurfaceKHR"));
        CHECK_NULLPTR(gpVkDestroySurfaceKHR, "Failed to load vkDestroySurfaceKHR")

        gpVkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkEnumeratePhysicalDevices"));
        CHECK_NULLPTR(gpVkEnumeratePhysicalDevices, "Failed to load vkEnumeratePhysicalDevices")

        gpVkGetPhysicalDeviceProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkGetPhysicalDeviceProperties"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceProperties, "Failed to load vkGetPhysicalDeviceProperties")

        gpVkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkGetPhysicalDeviceQueueFamilyProperties"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceQueueFamilyProperties, "Failed to load vkGetPhysicalDeviceQueueFamilyProperties")

        gpVkEnumerateDeviceExtensionProperties = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkEnumerateDeviceExtensionProperties"));
        CHECK_NULLPTR(gpVkEnumerateDeviceExtensionProperties, "Failed to load vkEnumerateDeviceExtensionProperties")

        gpVkGetPhysicalDeviceFeatures2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkGetPhysicalDeviceFeatures2"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceFeatures2, "Failed to load vkGetPhysicalDeviceFeatures2")

        gpVkCreateDevice = reinterpret_cast<PFN_vkCreateDevice>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCreateDevice"));
        CHECK_NULLPTR(gpVkCreateDevice, "Failed to load vkCreateDevice")

		gpVkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkGetDeviceQueue"));
		CHECK_NULLPTR(gpVkGetDeviceQueue, "Failed to load gpVkGetDeviceQueue")

        gpVkDestroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroyDevice"));
        CHECK_NULLPTR(gpVkDestroyDevice, "Failed to load vkDestroyDevice")

        gpVkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceSurfaceCapabilitiesKHR, "Failed to load vkGetPhysicalDeviceSurfaceCapabilitiesKHR")

        gpVkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceSurfaceFormatsKHR, "Failed to load vkGetPhysicalDeviceSurfaceFormatsKHR")

        gpVkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
        CHECK_NULLPTR(gpVkGetPhysicalDeviceSurfacePresentModesKHR, "Failed to load vkGetPhysicalDeviceSurfacePresentModesKHR")

        gpVkCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCreateSwapchainKHR"));
        CHECK_NULLPTR(gpVkCreateSwapchainKHR, "Failed to load vkCreateSwapchainKHR")

        gpVkDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroySwapchainKHR"));
        CHECK_NULLPTR(gpVkDestroySwapchainKHR, "Failed to load vkDestroySwapchainKHR")
        
        gpVkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkGetSwapchainImagesKHR"));
        CHECK_NULLPTR(gpVkGetSwapchainImagesKHR, "Failed to load gpVkGetSwapchainImagesKHR")

		gpVkCreateImageView = reinterpret_cast<PFN_vkCreateImageView>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCreateImageView"));
		CHECK_NULLPTR(gpVkCreateImageView, "Failed to load gpVkCreateImageView")
			
		gpVkDestroyImageView = reinterpret_cast<PFN_vkDestroyImageView>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroyImageView"));
		CHECK_NULLPTR(gpVkDestroyImageView, "Failed to load gpVkDestroyImageView")

		gpVkCreateBuffer = reinterpret_cast<PFN_vkCreateBuffer>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCreateBuffer"));
		CHECK_NULLPTR(gpVkCreateBuffer, "Failed to load gpVkCreateBuffer")

		gpVkDestroyBuffer = reinterpret_cast<PFN_vkDestroyBuffer>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroyBuffer"));
		CHECK_NULLPTR(gpVkDestroyBuffer, "Failed to load gpVkDestroyBuffer")
			
		gpVkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkGetPhysicalDeviceMemoryProperties"));
		CHECK_NULLPTR(gpVkGetPhysicalDeviceMemoryProperties, "Failed to load gpVkGetPhysicalDeviceMemoryProperties")

		gpVkGetBufferMemoryRequirements = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkGetBufferMemoryRequirements"));
		CHECK_NULLPTR(gpVkGetBufferMemoryRequirements, "Failed to load gpVkGetBufferMemoryRequirements")
			
		gpVkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkAllocateMemory"));
		CHECK_NULLPTR(gpVkAllocateMemory, "Failed to load gpVkAllocateMemory")

		gpVkBindBufferMemory = reinterpret_cast<PFN_vkBindBufferMemory>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkBindBufferMemory"));
		CHECK_NULLPTR(gpVkBindBufferMemory, "Failed to load gpVkBindBufferMemory")

		gpVkMapMemory = reinterpret_cast<PFN_vkMapMemory>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkMapMemory"));
		CHECK_NULLPTR(gpVkMapMemory, "Failed to load gpVkMapMemory")

		gpVkUnmapMemory = reinterpret_cast<PFN_vkUnmapMemory>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkUnmapMemory"));
		CHECK_NULLPTR(gpVkUnmapMemory, "Failed to load gpVkUnmapMemory")

		gpVkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkFreeMemory"));
		CHECK_NULLPTR(gpVkFreeMemory, "Failed to load gpVkFreeMemory")

		gpVkCreateCommandPool = reinterpret_cast<PFN_vkCreateCommandPool>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCreateCommandPool"));
		CHECK_NULLPTR(gpVkCreateCommandPool, "Failed to load gpVkCreateCommandPool")

		gpVkDestroyCommandPool = reinterpret_cast<PFN_vkDestroyCommandPool>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroyCommandPool"));
		CHECK_NULLPTR(gpVkDestroyCommandPool, "Failed to load gpVkDestroyCommandPool")

		gpVkAllocateCommandBuffers = reinterpret_cast<PFN_vkAllocateCommandBuffers>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkAllocateCommandBuffers"));
		CHECK_NULLPTR(gpVkAllocateCommandBuffers, "Failed to load gpVkAllocateCommandBuffers")

		gpVkFreeCommandBuffers = reinterpret_cast<PFN_vkFreeCommandBuffers>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkFreeCommandBuffers"));
		CHECK_NULLPTR(gpVkFreeCommandBuffers, "Failed to load gpVkFreeCommandBuffers")

		gpVkBeginCommandBuffer = reinterpret_cast<PFN_vkBeginCommandBuffer>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkBeginCommandBuffer"));
		CHECK_NULLPTR(gpVkBeginCommandBuffer, "Failed to load gpVkBeginCommandBuffer")

		gpVkCmdCopyBuffer = reinterpret_cast<PFN_vkCmdCopyBuffer>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCmdCopyBuffer"));
		CHECK_NULLPTR(gpVkCmdCopyBuffer, "Failed to load gpVkCmdCopyBuffer")

		gpVkEndCommandBuffer = reinterpret_cast<PFN_vkEndCommandBuffer>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkEndCommandBuffer"));
		CHECK_NULLPTR(gpVkEndCommandBuffer, "Failed to load gpVkEndCommandBuffer")

		gpVkCreateFence = reinterpret_cast<PFN_vkCreateFence>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCreateFence"));
		CHECK_NULLPTR(gpVkCreateFence, "Failed to load gpVkCreateFence")

		gpVkDestroyFence = reinterpret_cast<PFN_vkDestroyFence>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroyFence"));
		CHECK_NULLPTR(gpVkDestroyFence, "Failed to load gpVkDestroyFence")

		gpVkQueueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkQueueSubmit"));
		CHECK_NULLPTR(gpVkQueueSubmit, "Failed to load gpVkQueueSubmit")

		gpVkWaitForFences = reinterpret_cast<PFN_vkWaitForFences>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkWaitForFences"));
		CHECK_NULLPTR(gpVkWaitForFences, "Failed to load gpVkWaitForFences")

		gpVkCreateSemaphore = reinterpret_cast<PFN_vkCreateSemaphore>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCreateSemaphore"));
		CHECK_NULLPTR(gpVkCreateSemaphore, "Failed to load gpVkCreateSemaphore")

		gpVkDestroySemaphore = reinterpret_cast<PFN_vkDestroySemaphore>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroySemaphore"));
		CHECK_NULLPTR(gpVkDestroySemaphore, "Failed to load gpVkDestroySemaphore")

		gpVkCreateShaderModule = reinterpret_cast<PFN_vkCreateShaderModule>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCreateShaderModule"));
		CHECK_NULLPTR(gpVkCreateShaderModule, "Failed to load gpVkCreateShaderModule")

		gpVkCreatePipelineLayout = reinterpret_cast<PFN_vkCreatePipelineLayout>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCreatePipelineLayout"));
		CHECK_NULLPTR(gpVkCreatePipelineLayout, "Failed to load gpVkCreatePipelineLayout")

		gpVkCreateGraphicsPipelines = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCreateGraphicsPipelines"));
		CHECK_NULLPTR(gpVkCreateGraphicsPipelines, "Failed to load gpVkCreateGraphicsPipelines")

		gpVkDestroyPipeline = reinterpret_cast<PFN_vkDestroyPipeline>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroyPipeline"));
		CHECK_NULLPTR(gpVkDestroyPipeline, "Failed to load gpVkDestroyPipeline")

		gpVkDestroyPipelineLayout = reinterpret_cast<PFN_vkDestroyPipelineLayout>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroyPipelineLayout"));
		CHECK_NULLPTR(gpVkDestroyPipelineLayout, "Failed to load gpVkDestroyPipelineLayout")

		gpVkDestroyShaderModule = reinterpret_cast<PFN_vkDestroyShaderModule>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkDestroyShaderModule"));
		CHECK_NULLPTR(gpVkDestroyShaderModule, "Failed to load gpVkDestroyShaderModule")

		gpVkAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkAcquireNextImageKHR"));
		CHECK_NULLPTR(gpVkAcquireNextImageKHR, "Failed to load gpVkAcquireNextImageKHR")

		gpVkCmdPipelineBarrier2 = reinterpret_cast<PFN_vkCmdPipelineBarrier2>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCmdPipelineBarrier2"));
		CHECK_NULLPTR(gpVkCmdPipelineBarrier2, "Failed to load gpVkCmdPipelineBarrier2")

		gpVkCmdBindPipeline = reinterpret_cast<PFN_vkCmdBindPipeline>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCmdBindPipeline"));
		CHECK_NULLPTR(gpVkCmdBindPipeline, "Failed to load gpVkCmdBindPipeline")

		gpVkCmdBindIndexBuffer = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCmdBindIndexBuffer"));
		CHECK_NULLPTR(gpVkCmdBindIndexBuffer, "Failed to load gpVkCmdBindIndexBuffer")

		gpVkCmdBindVertexBuffers = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCmdBindVertexBuffers"));
		CHECK_NULLPTR(gpVkCmdBindVertexBuffers, "Failed to load gpVkCmdBindVertexBuffers")

		gpVkCmdSetViewport = reinterpret_cast<PFN_vkCmdSetViewport>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCmdSetViewport"));
		CHECK_NULLPTR(gpVkCmdSetViewport, "Failed to load gpVkCmdSetViewport")

		gpVkCmdSetScissor = reinterpret_cast<PFN_vkCmdSetScissor>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCmdSetScissor"));
		CHECK_NULLPTR(gpVkCmdSetScissor, "Failed to load gpVkCmdSetScissor")

		gpVkCmdBeginRendering = reinterpret_cast<PFN_vkCmdBeginRendering>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCmdBeginRendering"));
		CHECK_NULLPTR(gpVkCmdBeginRendering, "Failed to load gpVkCmdBeginRendering")

		gpVkCmdEndRendering = reinterpret_cast<PFN_vkCmdEndRendering>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCmdEndRendering"));
		CHECK_NULLPTR(gpVkCmdEndRendering, "Failed to load gpVkCmdEndRendering")
		
		gpVkCmdDrawIndexed = reinterpret_cast<PFN_vkCmdDrawIndexed>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkCmdDrawIndexed"));
		CHECK_NULLPTR(gpVkCmdDrawIndexed, "Failed to load gpVkCmdDrawIndexed")

		gpVkQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vkQueuePresentKHR"));
		CHECK_NULLPTR(gpVkQueuePresentKHR, "Failed to load gpVkQueuePresentKHR")

    }
}