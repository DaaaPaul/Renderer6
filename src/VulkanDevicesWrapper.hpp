#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanBackendWrapper.hpp"

class VulkanDevicesWrapper {
    friend class VulkanSwapchainWrapper;
    
    private:
    VulkanBackendWrapper* mVulkanBackendWrapper{};
    VkPhysicalDevice mPhysicalDevice{};
    VkDevice mLogicalDevice{};

    VkDeviceCreateInfo const* mLOGICAL_DEVICE_CREATE_INFO{};

    void arise();

    void static printEnabledFeaturesInVkFeatureStruct(void const* VK_FEATURE_STRUCT, const char* featureName);
    
    public:
    VulkanDevicesWrapper(VulkanBackendWrapper* givenVulkanBackendWrapper, VkPhysicalDevice const& GIVEN_PHYSICAL_DEVICE, VkDeviceCreateInfo const* GIVEN_LOGICAL_DEVICE_CREATE_INFO);
    ~VulkanDevicesWrapper();

    inline [[nodiscard]] VkPhysicalDevice getVkPhysicalDevice() noexcept { return mPhysicalDevice; }

    DELETE_COPY_CONSTRUCTORS(VulkanDevicesWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanDevicesWrapper)
};