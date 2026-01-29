#include <array>
#include <algorithm>
#include <iostream>
#include <climits>
#include <cassert>
#include "GlobalCreateInfos.h"
#include "VulkanPFNs.h"
#include "Common.h"

namespace GlobalCreateInfos {
    void fPopulateGlobalWindowCreateInfo() {
        GlobalCreateInfos::gWindowWidth = 800;
        GlobalCreateInfos::gWindowHeight = 600;
        GlobalCreateInfos::gWindowName = "Renderer6";
    }

    void fPopulateGlobalInstanceCreateInfo() {
        GlobalCreateInfos::gAppInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = nullptr,
            .applicationVersion = 0,
            .pEngineName = nullptr,
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_3,
        };
        GlobalCreateInfos::gEnabledLoaderLayers = { "VK_LAYER_KHRONOS_validation" };
        GlobalCreateInfos::gEnabledInstanceExtensions = GlobalCreateInfos::fGetGlfwWindowExtensions();
        VkInstanceCreateFlags instanceCreateFlags = 0;
        #ifdef __APPLE__
        GlobalCreateInfos::gEnabledInstanceExtensions.push_back("VK_KHR_portability_enumeration");
        instanceCreateFlags += VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        #endif
        GlobalCreateInfos::gInstanceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = instanceCreateFlags,
            .pApplicationInfo = &GlobalCreateInfos::gAppInfo,
            .enabledLayerCount = static_cast<uint32_t>(GlobalCreateInfos::gEnabledLoaderLayers.size()),
            .ppEnabledLayerNames = GlobalCreateInfos::gEnabledLoaderLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(GlobalCreateInfos::gEnabledInstanceExtensions.size()),
            .ppEnabledExtensionNames = GlobalCreateInfos::gEnabledInstanceExtensions.data(),
        };
    }

    void fPopulateGlobalSharedPhysicalLogicalDeviceInfo() {
        GlobalCreateInfos::gDeviceEnabledExtendedDynamicStateFeatures = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
            .extendedDynamicState2 = true
        };
        GlobalCreateInfos::gDeviceEnabledDynamicRenderingFeatures = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = &GlobalCreateInfos::gDeviceEnabledExtendedDynamicStateFeatures,
            .dynamicRendering = true
        };
        GlobalCreateInfos::gDeviceEnabledSyncFeatures = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext = &GlobalCreateInfos::gDeviceEnabledDynamicRenderingFeatures,
            .synchronization2 = true
        };
        GlobalCreateInfos::gEnabledDeviceFeatures = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &GlobalCreateInfos::gDeviceEnabledSyncFeatures,
            .features = {
                .samplerAnisotropy = true
            }
        };

        GlobalCreateInfos::gEnabledDeviceExtensions = {
            "VK_KHR_swapchain",
            "VK_KHR_synchronization2",
            "VK_KHR_spirv_1_4",
            #ifdef __APPLE__
            "VK_KHR_portability_subset"
            #endif
        };

        GlobalCreateInfos::gDeviceQueueFamilyQueuePriorities = {
            {0.5f}
        };

        GlobalCreateInfos::gDeviceQueueFamilyCreateInfos = {
            VkDeviceQueueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = UINT32_MAX, // warning value: this must be set non-absurdly later
                .queueCount = 1,
                .pQueuePriorities = GlobalCreateInfos::gDeviceQueueFamilyQueuePriorities[0].data()
            }
        };

        assert(GlobalCreateInfos::gDeviceQueueFamilyQueuePriorities.size() == GlobalCreateInfos::gDeviceQueueFamilyCreateInfos.size());
        for(int i = 0; i < gDeviceQueueFamilyQueuePriorities.size(); i++) {
            assert(GlobalCreateInfos::gDeviceQueueFamilyQueuePriorities[i].size() == GlobalCreateInfos::gDeviceQueueFamilyCreateInfos[i].queueCount);
        }
    }

    void fPopulateGlobalSelectedPhysicalDevice(VkInstance createdInstance) {
        GlobalCreateInfos::gSelectedPhysicalDevice = GlobalCreateInfos::fSelectPhysicalDevice(createdInstance);
    }

    void fPopulateGlobalLogicalDeviceCreateInfo() {
        // here i am: set it to a non-absurd value
        GlobalCreateInfos::gDeviceQueueFamilyCreateInfos[0].queueFamilyIndex = fGetGraphicsQueueFamilyIndex(gSelectedPhysicalDevice); 

        for(int i = 0; i < GlobalCreateInfos::gDeviceQueueFamilyCreateInfos.size(); i++) {
            assert(GlobalCreateInfos::gDeviceQueueFamilyCreateInfos[i].queueFamilyIndex != UINT32_MAX);
        }

        GlobalCreateInfos::gLogicalDeviceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &GlobalCreateInfos::gEnabledDeviceFeatures,
            .flags = 0,
            .queueCreateInfoCount = static_cast<uint32_t>(GlobalCreateInfos::gDeviceQueueFamilyCreateInfos.size()),
            .pQueueCreateInfos = GlobalCreateInfos::gDeviceQueueFamilyCreateInfos.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(GlobalCreateInfos::gEnabledDeviceExtensions.size()),
            .ppEnabledExtensionNames = GlobalCreateInfos::gEnabledDeviceExtensions.data(),
            .pEnabledFeatures = nullptr
        };
    }

    [[nodiscard]] VkPhysicalDevice fSelectPhysicalDevice(VkInstance instance) {
        uint32_t physicalDeviceCount{};
        CHECK_VK_SUCCESS(VulkanPFNs::gpVkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));
        std::vector<VkPhysicalDevice> systemPhysicalDevices(physicalDeviceCount);
        CHECK_VK_SUCCESS(VulkanPFNs::gpVkEnumeratePhysicalDevices(instance, &physicalDeviceCount, systemPhysicalDevices.data()));

        // weed out the downs
        for(int i = 0; i < systemPhysicalDevices.size(); i++) {
            VkPhysicalDeviceProperties physicalDeviceProperties{};
		    VulkanPFNs::gpVkGetPhysicalDeviceProperties(systemPhysicalDevices[i], &physicalDeviceProperties);

            // api version check
            if(!(physicalDeviceProperties.apiVersion >= gAppInfo.apiVersion)) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
                std::cout << "Weeded out " << physicalDeviceProperties.deviceName << " for too low API version of below " << UINT32_TO_VK_API_VERSION_CSTR(gAppInfo.apiVersion) << "\n";
                continue;
            }

            // has graphics queue family with enough queues check
            uint32_t physicalDeviceQueueFamilyCount{};
            VulkanPFNs::gpVkGetPhysicalDeviceQueueFamilyProperties(systemPhysicalDevices[i], &physicalDeviceQueueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> physicalDeviceQueueFamilyProperties(physicalDeviceQueueFamilyCount);
            VulkanPFNs::gpVkGetPhysicalDeviceQueueFamilyProperties(systemPhysicalDevices[i], &physicalDeviceQueueFamilyCount, physicalDeviceQueueFamilyProperties.data());

            bool foundGraphicsQueueFamilyWithEnoughQueues = false;
            for(int i = 0; i < physicalDeviceQueueFamilyCount; i++) {
                if ((physicalDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && 
                    (physicalDeviceQueueFamilyProperties[i].queueCount >= GlobalCreateInfos::gDeviceQueueFamilyCreateInfos[0].queueCount)) {
                    foundGraphicsQueueFamilyWithEnoughQueues = true;
                }
            }

            if(!foundGraphicsQueueFamilyWithEnoughQueues) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
                std::cout << "Weeded out " << physicalDeviceProperties.deviceName << " for not having a graphics queue family with eough queues" << "\n";
                continue;
            }

            // extensions check
            uint32_t physicalDeviceExtensionsCount{};
            VulkanPFNs::gpVkEnumerateDeviceExtensionProperties(systemPhysicalDevices[i], nullptr, &physicalDeviceExtensionsCount, nullptr);
            std::vector<VkExtensionProperties> physicalDeviceExtensions(physicalDeviceExtensionsCount);
            VulkanPFNs::gpVkEnumerateDeviceExtensionProperties(systemPhysicalDevices[i], nullptr, &physicalDeviceExtensionsCount, physicalDeviceExtensions.data());

            std::vector<std::string> physicalDeviceExtensionNames{};
            for(VkExtensionProperties const& physicalDeviceExtension : physicalDeviceExtensions) {
                physicalDeviceExtensionNames.push_back(physicalDeviceExtension.extensionName);
            }
            std::vector<std::string> logicalDeviceExtensionNames{};
            for(int i = 0; i < GlobalCreateInfos::gEnabledDeviceExtensions.size(); i++) {
                logicalDeviceExtensionNames.push_back(GlobalCreateInfos::gEnabledDeviceExtensions[i]);
            }

            if(!Common::containsAll(physicalDeviceExtensionNames, logicalDeviceExtensionNames)) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
                std::cout << "Weeded out " << physicalDeviceProperties.deviceName << " for not having the device extensions this application needs" << "\n";
                continue;
            }

            // features check
            VkPhysicalDeviceExtendedDynamicState2FeaturesEXT physicalDeviceExtendedDynamicStateFeaturesStatus{ 
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT 
            };
            VkPhysicalDeviceDynamicRenderingFeatures physicalDeviceDynamicRenderingFeaturesStatus{ 
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
                .pNext = &physicalDeviceExtendedDynamicStateFeaturesStatus
            };
            VkPhysicalDeviceSynchronization2Features physicalDeviceSyncFeaturesStatus{ 
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
                .pNext = &physicalDeviceDynamicRenderingFeaturesStatus
            };
            VkPhysicalDeviceFeatures2 physicalDeviceFeaturesStatus{ 
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
                .pNext = &physicalDeviceSyncFeaturesStatus
            };
            VulkanPFNs::gpVkGetPhysicalDeviceFeatures2(systemPhysicalDevices[i], &physicalDeviceFeaturesStatus);

            if(!(physicalDeviceFeaturesStatus.features.samplerAnisotropy && physicalDeviceSyncFeaturesStatus.synchronization2 && physicalDeviceDynamicRenderingFeaturesStatus.dynamicRendering && physicalDeviceExtendedDynamicStateFeaturesStatus.extendedDynamicState2)) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
                std::cout << "Weeded out " << physicalDeviceProperties.deviceName << " for not having the device features this application needs" << "\n";
                continue;
            }
        }

        if(systemPhysicalDevices.empty()) {
            throw std::runtime_error("There is no suitable GPU on your computer for this application");
        }

        // perform nice-to-have checks on the remaining physical devices
        // index 0 : are you a discrete GPU?
        std::vector<std::array<uint16_t, 1>> niceToHavesBySystemPhysicalDevice(systemPhysicalDevices.size(), { 0 });
        for(int i = 0; i < systemPhysicalDevices.size(); i++) {
            VkPhysicalDeviceProperties physicalDeviceProperties{};
		    VulkanPFNs::gpVkGetPhysicalDeviceProperties(systemPhysicalDevices[i], &physicalDeviceProperties);
            
            if(physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                niceToHavesBySystemPhysicalDevice[i][0] = 1;
            }
        }

        std::vector<uint16_t> niceToHaveSumsBySystemPhysicalDevice(systemPhysicalDevices.size(), 0);
        for(int i = 0; i < systemPhysicalDevices.size(); i++) {
            for(uint16_t const& niceToHaveStatus : niceToHavesBySystemPhysicalDevice[i]) {
                niceToHaveSumsBySystemPhysicalDevice[i] += niceToHaveStatus;
            }
        }

        size_t selectedPhysicalDeviceIndex = std::distance(niceToHaveSumsBySystemPhysicalDevice.begin(), std::max_element(niceToHaveSumsBySystemPhysicalDevice.begin(), niceToHaveSumsBySystemPhysicalDevice.end()));
        VkPhysicalDevice selectedPhysicalDevice = systemPhysicalDevices[selectedPhysicalDeviceIndex];
        VkPhysicalDeviceProperties selectedPhysicalDeviceProperties{};
	    VulkanPFNs::gpVkGetPhysicalDeviceProperties(selectedPhysicalDevice, &selectedPhysicalDeviceProperties);
        std::cout << "SELECTED PHYSICAL DEVICE: " << selectedPhysicalDeviceProperties.deviceName << "\n";
        std::cout << "DISCRETE GPU? " << ((niceToHavesBySystemPhysicalDevice[selectedPhysicalDeviceIndex][0]) ? "Yes\n" : "No\n");

        return selectedPhysicalDevice;
    }

    void fPopulateGlobalSwapchainKHRCreateInfo(VkPhysicalDevice physicalDevice, VkSurfaceKHR surfaceKHR, GLFWwindow* window) {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        CHECK_VK_SUCCESS(VulkanPFNs::gpVkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surfaceKHR, &surfaceCapabilities));

        VkExtent2D surfaceExtentInPixels{};
        if(surfaceCapabilities.currentExtent.width == UINT32_MAX || surfaceCapabilities.currentExtent.height == UINT32_MAX) {
            glfwGetFramebufferSize(window, reinterpret_cast<int*>(&surfaceExtentInPixels.width), reinterpret_cast<int*>(&surfaceExtentInPixels.height));
        } else {
            surfaceExtentInPixels = VkExtent2D(surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
        }

        GlobalCreateInfos::gGraphicsQueueFamilyIndex = fGetGraphicsQueueFamilyIndex(physicalDevice);
        GlobalCreateInfos::gSwapchainKHRCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = surfaceKHR,
            .minImageCount = 4,
            .imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
            .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
            .imageExtent = surfaceExtentInPixels,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &GlobalCreateInfos::gGraphicsQueueFamilyIndex,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE,
        };
    }


    [[nodiscard]] uint32_t fGetGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice) {
        uint32_t physicalDeviceQueueFamilyCount{};
        VulkanPFNs::gpVkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &physicalDeviceQueueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> physicalDeviceQueueFamilyProperties(physicalDeviceQueueFamilyCount);
        VulkanPFNs::gpVkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &physicalDeviceQueueFamilyCount, physicalDeviceQueueFamilyProperties.data());

        uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
        for(int i = 0; i < physicalDeviceQueueFamilyCount; i++) {
            if (physicalDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsQueueFamilyIndex = i;
            }
        }

        if(graphicsQueueFamilyIndex == UINT32_MAX) {
            throw std::runtime_error("Did not find a graphics queue for physical device");
        } else {
            return graphicsQueueFamilyIndex;
        }
    }

    [[nodiscard]] std::vector<const char*> fGetGlfwWindowExtensions() {
        uint32_t requiredGlfwExtensionsCount{};
        const char** requiredGlfwExtensionsNames = glfwGetRequiredInstanceExtensions(&requiredGlfwExtensionsCount);
        std::vector<const char*> requiredGlfwExtensionsNamesVector{};

        if(!requiredGlfwExtensionsNames) {
            #ifdef __APPLE__
            requiredGlfwExtensionsNamesVector.push_back("VK_KHR_surface");
            requiredGlfwExtensionsNamesVector.push_back("VK_EXT_metal_surface");
            #endif

            #ifdef _WIN32
            CHECK_NULLPTR(requiredGlfwExtensionsNames)
            #endif
        }   

        for(int i = 0; i < requiredGlfwExtensionsCount; i++) {
            requiredGlfwExtensionsNamesVector.push_back(requiredGlfwExtensionsNames[i]);
        }   

        return requiredGlfwExtensionsNamesVector;
    }
}