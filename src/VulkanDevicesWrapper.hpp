#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanBackendWrapper.hpp"

struct VulkanDevicesWrapper {
    struct VulkanDevicesWrapperConstructParameters {
        VkPhysicalDevice selectedPhysicalDevice{};
        VkDeviceCreateInfo logicalDeviceCreateInfo{};
        std::vector<VkDeviceQueueCreateInfo> deviceQueueFamilyCreateInfos{};
        std::vector<std::vector<float>> deviceQueueFamilyQueuePriorities{};
        std::vector<const char*> enabledDeviceExtensions{};
        VkPhysicalDeviceExtendedDynamicState2FeaturesEXT deviceEnabledExtendedDynamicStateFeatures{};
        VkPhysicalDeviceDynamicRenderingFeatures deviceEnabledDynamicRenderingFeatures{};
        VkPhysicalDeviceSynchronization2Features deviceEnabledSyncFeatures{};
        VkPhysicalDeviceFeatures2 enabledDeviceFeatures{};
    };

    VulkanBackendWrapper* mVulkanBackendWrapper{};
    VkPhysicalDevice mPhysicalDevice{};
    VkDevice mLogicalDevice{};
	std::vector<VkQueue> mGraphicsFamilyQueues{};
    VulkanDevicesWrapperConstructParameters mParameters{};

    [[nodiscard]] static VulkanDevicesWrapperConstructParameters getConstructParameters(VkInstance instance);
	[[nodiscard]] static uint32_t getGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice);
    VulkanDevicesWrapper(VulkanBackendWrapper* givenVulkanBackendWrapper, VulkanDevicesWrapperConstructParameters const& GIVEN_VULKAN_DEVICES_WRAPPER_CONSTRUCT_PARAMETERS);
    ~VulkanDevicesWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanDevicesWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanDevicesWrapper)
};