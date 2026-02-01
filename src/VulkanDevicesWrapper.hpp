#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanBackendWrapper.hpp"

struct VulkanDevicesWrapper {
    struct VulkanDevicesWrapperConstructParameters {
        VkPhysicalDevice mSelectedPhysicalDevice{};
        VkDeviceCreateInfo mLogicalDeviceCreateInfo{};
        std::vector<VkDeviceQueueCreateInfo> mDeviceQueueFamilyCreateInfos{};
        std::vector<std::vector<float>> mDeviceQueueFamilyQueuePriorities{};
        std::vector<const char*> mEnabledDeviceExtensions{};
        VkPhysicalDeviceExtendedDynamicState2FeaturesEXT mDeviceEnabledExtendedDynamicStateFeatures{};
        VkPhysicalDeviceDynamicRenderingFeatures mDeviceEnabledDynamicRenderingFeatures{};
        VkPhysicalDeviceSynchronization2Features mDeviceEnabledSyncFeatures{};
        VkPhysicalDeviceFeatures2 mEnabledDeviceFeatures{};
    };

    VulkanBackendWrapper* mpVulkanBackendWrapper{};
    VkPhysicalDevice mpPhysicalDevice{};
    VkDevice mpLogicalDevice{};
	std::vector<VkQueue> mGraphicsFamilypQueues{};
    VulkanDevicesWrapperConstructParameters mParameters{};

    [[nodiscard]] static VulkanDevicesWrapperConstructParameters getConstructParameters(VkInstance instance);
	[[nodiscard]] static uint32_t getGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice);
    explicit VulkanDevicesWrapper(VulkanBackendWrapper* givenVulkanBackendWrapper, VulkanDevicesWrapperConstructParameters const& GIVEN_VULKAN_DEVICES_WRAPPER_CONSTRUCT_PARAMETERS);
    ~VulkanDevicesWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanDevicesWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanDevicesWrapper)
};