#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace VulkanPFNs {
    void fSetInstance(VkInstance const& givenInstance);
    void fLoadVkGetInstanceProcAddr();
    void fLoadTrueGlobalVulkanFunctions();
    void fLoadVulkanFunctions();

    inline VkInstance gInstanceInUse{};

    inline PFN_vkGetInstanceProcAddr gpVkGetInstanceProcAddr{};
    inline PFN_vkEnumerateInstanceExtensionProperties gpVkEnumerateInstanceExtensionProperties{};
    inline PFN_vkEnumerateInstanceLayerProperties gpVkEnumerateInstanceLayerProperties{};
    inline PFN_vkCreateInstance gpVkCreateInstance{};
    inline PFN_vkDestroyInstance gpVkDestroyInstance{};
}