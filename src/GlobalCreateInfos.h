#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "VulkanBackendWrapper.hpp"

namespace GlobalCreateInfos {
    inline VkSwapchainCreateInfoKHR gSwapchainKHRCreateInfo{};
    void fPopulateGlobalSwapchainKHRCreateInfo(VkPhysicalDevice physicalDevice, VkSurfaceKHR surfaceKHR, GLFWwindow* window);
}