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

#define LOAD_INSTANCE_PFN(shortName) \
        gpVk##shortName = reinterpret_cast<PFN_vk##shortName>(gpVkGetInstanceProcAddr(gpInstanceUsed, "vk" #shortName)); \
        CHECK_NULLPTR(gpVk##shortName, "Failed to load " "vk" #shortName)

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

		LOAD_INSTANCE_PFN(DestroyInstance)
		LOAD_INSTANCE_PFN(DestroySurfaceKHR)
		LOAD_INSTANCE_PFN(EnumeratePhysicalDevices)
		LOAD_INSTANCE_PFN(GetPhysicalDeviceProperties)
		LOAD_INSTANCE_PFN(GetPhysicalDeviceQueueFamilyProperties)
		LOAD_INSTANCE_PFN(EnumerateDeviceExtensionProperties)
		LOAD_INSTANCE_PFN(GetPhysicalDeviceFeatures2)
		LOAD_INSTANCE_PFN(CreateDevice)
		LOAD_INSTANCE_PFN(GetDeviceQueue)
		LOAD_INSTANCE_PFN(DestroyDevice)
		LOAD_INSTANCE_PFN(GetPhysicalDeviceSurfaceCapabilitiesKHR)
		LOAD_INSTANCE_PFN(GetPhysicalDeviceSurfaceFormatsKHR)
		LOAD_INSTANCE_PFN(GetPhysicalDeviceSurfacePresentModesKHR)
		LOAD_INSTANCE_PFN(CreateSwapchainKHR)
		LOAD_INSTANCE_PFN(DestroySwapchainKHR)
		LOAD_INSTANCE_PFN(GetSwapchainImagesKHR)
		LOAD_INSTANCE_PFN(CreateImageView)
		LOAD_INSTANCE_PFN(DestroyImageView)
		LOAD_INSTANCE_PFN(CreateBuffer)
		LOAD_INSTANCE_PFN(DestroyBuffer)
		LOAD_INSTANCE_PFN(GetPhysicalDeviceMemoryProperties)
		LOAD_INSTANCE_PFN(GetBufferMemoryRequirements)
		LOAD_INSTANCE_PFN(AllocateMemory)
		LOAD_INSTANCE_PFN(BindBufferMemory)
		LOAD_INSTANCE_PFN(MapMemory)
		LOAD_INSTANCE_PFN(UnmapMemory)
		LOAD_INSTANCE_PFN(FreeMemory)
		LOAD_INSTANCE_PFN(CreateCommandPool)
		LOAD_INSTANCE_PFN(DestroyCommandPool)
		LOAD_INSTANCE_PFN(AllocateCommandBuffers)
		LOAD_INSTANCE_PFN(FreeCommandBuffers)
		LOAD_INSTANCE_PFN(BeginCommandBuffer)
		LOAD_INSTANCE_PFN(CmdCopyBuffer)
		LOAD_INSTANCE_PFN(EndCommandBuffer)
		LOAD_INSTANCE_PFN(CreateFence)
		LOAD_INSTANCE_PFN(DestroyFence)
		LOAD_INSTANCE_PFN(QueueSubmit)
		LOAD_INSTANCE_PFN(WaitForFences)
		LOAD_INSTANCE_PFN(CreateSemaphore)
		LOAD_INSTANCE_PFN(DestroySemaphore)
		LOAD_INSTANCE_PFN(CreateShaderModule)
		LOAD_INSTANCE_PFN(CreatePipelineLayout)
		LOAD_INSTANCE_PFN(CreateGraphicsPipelines)
		LOAD_INSTANCE_PFN(DestroyPipeline)
		LOAD_INSTANCE_PFN(DestroyPipelineLayout)
		LOAD_INSTANCE_PFN(DestroyShaderModule)
		LOAD_INSTANCE_PFN(AcquireNextImageKHR)
		LOAD_INSTANCE_PFN(CmdPipelineBarrier2)
		LOAD_INSTANCE_PFN(CmdBindPipeline)
		LOAD_INSTANCE_PFN(CmdBindIndexBuffer)
		LOAD_INSTANCE_PFN(CmdBindVertexBuffers)
		LOAD_INSTANCE_PFN(CmdSetViewport)
		LOAD_INSTANCE_PFN(CmdSetScissor)
		LOAD_INSTANCE_PFN(CmdBeginRendering)
		LOAD_INSTANCE_PFN(CmdEndRendering)
		LOAD_INSTANCE_PFN(CmdDrawIndexed)
		LOAD_INSTANCE_PFN(QueuePresentKHR)
		LOAD_INSTANCE_PFN(ResetFences)
		LOAD_INSTANCE_PFN(DeviceWaitIdle)
		LOAD_INSTANCE_PFN(ResetCommandBuffer)
		LOAD_INSTANCE_PFN(CreateDescriptorSetLayout)
		LOAD_INSTANCE_PFN(AllocateDescriptorSets)
		LOAD_INSTANCE_PFN(CreateDescriptorPool)
		
    }
}