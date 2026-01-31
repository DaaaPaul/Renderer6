#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace VulkanPFNs {
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
    inline PFN_vkCreateDevice gpVkCreateDevice{};
    inline PFN_vkDestroyDevice gpVkDestroyDevice{};
    inline PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR gpVkGetPhysicalDeviceSurfaceCapabilitiesKHR{};
    inline PFN_vkGetPhysicalDeviceSurfaceFormatsKHR gpVkGetPhysicalDeviceSurfaceFormatsKHR{};
    inline PFN_vkGetPhysicalDeviceSurfacePresentModesKHR gpVkGetPhysicalDeviceSurfacePresentModesKHR{};
    inline PFN_vkCreateSwapchainKHR gpVkCreateSwapchainKHR{};
    inline PFN_vkDestroySwapchainKHR gpVkDestroySwapchainKHR{};
    inline PFN_vkGetSwapchainImagesKHR gpVkGetSwapchainImagesKHR{};
	inline PFN_vkCreateImageView gpVkCreateImageView{};
	inline PFN_vkDestroyImageView gpVkDestroyImageView{};
}