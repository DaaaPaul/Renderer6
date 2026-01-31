#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanDevicesWrapper.hpp"

struct VulkanSwapchainWrapper {
    struct VulkanSwapchainWrapperConstructInfo {
        const VkSurfaceKHR const mSURFACE_KHR{};
        VkSwapchainCreateInfoKHR mSwapchainKHRCreateInfo{};
        const uint32_t mGRAPHICS_QUEUE_FAMILY_INDEX{};
    };

    VulkanDevicesWrapper* mVulkanDevicesWrapper{};
    VkSwapchainKHR mSwapchainKHR{};
    VkSurfaceKHR mSurfaceKHR{};
    std::vector<VkImageView> mImageViews{};
    VulkanSwapchainWrapperConstructInfo mParameters{};

    [[nodiscard]] static VulkanSwapchainWrapperConstructInfo getConstructParameters(VkInstance instance, VkPhysicalDevice physicalDevice, GLFWwindow* window);
    static void checkHaveVkFormatColorspace(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE);
    static void checkHavePresentModeKHR(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE);

    VulkanSwapchainWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper, VulkanSwapchainWrapperConstructInfo const& GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO);
    ~VulkanSwapchainWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanSwapchainWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanSwapchainWrapper)
};