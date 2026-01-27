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
    inline PFN_vkDestroySurfaceKHR gpVkDestroySurfaceKHR{};
    inline PFN_vkEnumeratePhysicalDevices gpVkEnumeratePhysicalDevices{};
    inline PFN_vkGetPhysicalDeviceProperties gpVkGetPhysicalDeviceProperties{};
    inline PFN_vkGetPhysicalDeviceQueueFamilyProperties gpVkGetPhysicalDeviceQueueFamilyProperties{};
    inline PFN_vkEnumerateDeviceExtensionProperties gpVkEnumerateDeviceExtensionProperties{};
    inline PFN_vkGetPhysicalDeviceFeatures2 gpVkGetPhysicalDeviceFeatures2{};
}