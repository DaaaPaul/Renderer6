#include <array>
#include <algorithm>
#include <iostream>
#include <climits>
#include <cassert>
#include "GlobalCreateInfos.h"
#include "VulkanPFNs.h"
#include "Common.h"

namespace GlobalCreateInfos {
    void fPopulateGlobalSwapchainKHRCreateInfo(VkPhysicalDevice physicalDevice, VkSurfaceKHR surfaceKHR, GLFWwindow* window) {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        CHECK_VK_SUCCESS(
        VulkanPFNs::gpVkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surfaceKHR, &surfaceCapabilities),
        "Failed to get physical device surface capabilities"
        );

        VkExtent2D surfaceExtentInPixels{};
        if(surfaceCapabilities.currentExtent.width == UINT32_MAX || surfaceCapabilities.currentExtent.height == UINT32_MAX) {
            glfwGetFramebufferSize(window, reinterpret_cast<int*>(&surfaceExtentInPixels.width), reinterpret_cast<int*>(&surfaceExtentInPixels.height));
        } else {
            surfaceExtentInPixels = VkExtent2D(surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
        }

        GlobalCreateInfos::gGraphicsQueueFamilyIndex = fGetGraphicsQueueFamilyIndex(physicalDevice);
        GlobalCreateInfos::gSwapchainKHRCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = surfaceKHR,
            .minImageCount = 4,
            .imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
            .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
            .imageExtent = surfaceExtentInPixels,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &GlobalCreateInfos::gGraphicsQueueFamilyIndex,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE,
        };
    }
}