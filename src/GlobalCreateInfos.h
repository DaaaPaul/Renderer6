#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "VulkanBackendWrapper.hpp"

namespace GlobalCreateInfos {
    inline VkInstanceCreateInfo gInstanceCreateInfo{};
    inline VkApplicationInfo gAppInfo{};
    inline std::vector<const char*> gEnabledLoaderLayers{};
    inline std::vector<const char*> gEnabledInstanceExtensions{};
    void fPopulateGlobalInstanceCreateInfo();

    inline VkPhysicalDevice gSelectedPhysicalDevice{};
    void fPopulateGlobalSharedPhysicalLogicalDeviceInfo();
    void fPopulateGlobalSelectedPhysicalDevice(VkInstance createdInstance);

    inline VkDeviceCreateInfo gLogicalDeviceCreateInfo{};
    inline std::vector<VkDeviceQueueCreateInfo> gDeviceQueueFamilyCreateInfos{};
    inline std::vector<std::vector<float>> gDeviceQueueFamilyQueuePriorities{};
    inline std::vector<const char*> gEnabledDeviceExtensions{};
    inline VkPhysicalDeviceExtendedDynamicState2FeaturesEXT gDeviceEnabledExtendedDynamicStateFeatures{};
    inline VkPhysicalDeviceDynamicRenderingFeatures gDeviceEnabledDynamicRenderingFeatures{};
    inline VkPhysicalDeviceSynchronization2Features gDeviceEnabledSyncFeatures{};
    inline VkPhysicalDeviceFeatures2 gEnabledDeviceFeatures{};
    void fPopulateGlobalLogicalDeviceCreateInfo();

    inline VkSwapchainCreateInfoKHR gSwapchainKHRCreateInfo{};
    inline uint32_t gGraphicsQueueFamilyIndex{};
    void fPopulateGlobalSwapchainKHRCreateInfo(VkPhysicalDevice physicalDevice, VkSurfaceKHR surfaceKHR, GLFWwindow* window);

    [[nodiscard]] VkPhysicalDevice fSelectPhysicalDevice(VkInstance instance);
    [[nodiscard]] uint32_t fGetGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice);
}