#include <array>
#include <algorithm>
#include "GlobalCreateInfos.h"
#include "VulkanPFNs.h"
#include "Common.h"

namespace GlobalCreateInfos {
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

        return systemPhysicalDevices[std::distance(niceToHaveSumsBySystemPhysicalDevice.begin(), std::max_element(niceToHaveSumsBySystemPhysicalDevice.begin(), niceToHaveSumsBySystemPhysicalDevice.end()))];
    }

    [[nodiscard]] uint32_t fGetPhysicalDeviceGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice) {
        
    }
}