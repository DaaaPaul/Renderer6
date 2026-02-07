#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanDevicesWrapper.hpp"

struct VulkanSwapchainWrapper {
    struct VulkanSwapchainWrapperConstructInfo {
        VkSurfaceKHR mpSurfaceKHR{};
        VkSwapchainCreateInfoKHR mSwapchainKHRCreateInfo{};
        const uint32_t mGRAPHICS_QUEUE_FAMILY_INDEX{};
    };

    VulkanDevicesWrapper* mpVulkanDevicesWrapper{};
    VkSwapchainKHR mpSwapchainKHR{};
    VkSurfaceKHR mpSurfaceKHR{};
    VulkanSwapchainWrapperConstructInfo mParameters{};

	void recreateThyself();

    [[nodiscard]] static VulkanSwapchainWrapperConstructInfo getConstructParameters(VkInstance pInstance, VkPhysicalDevice pPhysicalDevice, GLFWwindow* pGlfwWindow);
    static void checkHaveVkFormatColorspace(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE);
    static void checkHavePresentModeKHR(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE);

    explicit VulkanSwapchainWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper, VulkanSwapchainWrapperConstructInfo const& GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO);
    ~VulkanSwapchainWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanSwapchainWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanSwapchainWrapper)
};