#include <array>
#include <algorithm>
#include <iostream>
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

    void fPopulateGlobalSelectedPhysicalDevice(VkInstance createdInstance) {
        GlobalCreateInfos::gSelectedPhysicalDevice = GlobalCreateInfos::fSelectPhysicalDevice(createdInstance);
    }

    void fPopulateGlobalLogicalDeviceCreateInfo() {
        GlobalCreateInfos::gEnabledDeviceExtensions = {
            "VK_KHR_swapchain", 
            "VK_KHR_synchronization2", 
            "VK_KHR_spirv_1_4"
        };

        VkPhysicalDeviceExtendedDynamicState2FeaturesEXT deviceEnabledExtendedDynamicStateFeatures{ 
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
            .extendedDynamicState2 = true
        };
        VkPhysicalDeviceDynamicRenderingFeatures deviceEnabledDynamicRenderingFeatures{ 
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = &deviceEnabledExtendedDynamicStateFeatures,
            .dynamicRendering = true
        };
        VkPhysicalDeviceSynchronization2Features deviceEnabledSyncFeatures{ 
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext = &deviceEnabledDynamicRenderingFeatures,
            .synchronization2 = true
        };
        GlobalCreateInfos::gEnabledDeviceFeatures = { 
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &deviceEnabledSyncFeatures,
            .features = { 
                .samplerAnisotropy = true 
            }
        };

        GlobalCreateInfos::gDeviceQueueFamilyQueuePriorities = {
            {0.5f}
        };
        GlobalCreateInfos::gDeviceQueueFamilyCreateInfos = {
            VkDeviceQueueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = GlobalCreateInfos::fGetGraphicsQueueFamilyIndex(GlobalCreateInfos::gSelectedPhysicalDevice),
                .queueCount = 1,
                .pQueuePriorities = GlobalCreateInfos::gDeviceQueueFamilyQueuePriorities[0].data()
            }
        };
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

    [[nodiscard]] std::vector<const char*> fGetGlfwWindowExtensions() {
        glfwInit();

        uint32_t requiredGlfwExtensionsCount{};
        const char** requiredGlfwExtensionsNames = glfwGetRequiredInstanceExtensions(&requiredGlfwExtensionsCount);

        CHECK_NULLPTR(requiredGlfwExtensionsNames)

        std::vector<const char*> requiredGlfwExtensionsNamesVector{};
        for(int i = 0; i < requiredGlfwExtensionsCount; i++) {
            requiredGlfwExtensionsNamesVector.push_back(requiredGlfwExtensionsNames[i]);
        }

        return requiredGlfwExtensionsNamesVector;
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
                    (physicalDeviceQueueFamilyProperties[i].queueCount >= gLogicalDeviceCreateInfo.pQueueCreateInfos[0].queueCount)) {
                    foundGraphicsQueueFamilyWithEnoughQueues = true;
                }
            }

            if(!foundGraphicsQueueFamilyWithEnoughQueues) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
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
            for(int i = 0; i < gLogicalDeviceCreateInfo.enabledExtensionCount; i++) {
                logicalDeviceExtensionNames.push_back(gLogicalDeviceCreateInfo.ppEnabledExtensionNames[i]);
            }

            if(!Common::containsAll(physicalDeviceExtensionNames, logicalDeviceExtensionNames)) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
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
            VulkanPFNs::gpVkGetPhysicalDeviceFeatures2KHR(systemPhysicalDevices[i], &physicalDeviceFeaturesStatus);

            if(!(physicalDeviceFeaturesStatus.features.samplerAnisotropy && physicalDeviceSyncFeaturesStatus.synchronization2 && physicalDeviceDynamicRenderingFeaturesStatus.dynamicRendering && physicalDeviceExtendedDynamicStateFeaturesStatus.extendedDynamicState2)) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
                continue;
            }
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
        std::cout << "DISCRETE GPU? " << (niceToHavesBySystemPhysicalDevice[selectedPhysicalDeviceIndex][0]) ? "Yes\n" : "No\n";

        return selectedPhysicalDevice;
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
}