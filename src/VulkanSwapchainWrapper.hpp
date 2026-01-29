#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanDevicesWrapper.hpp"

class VulkanSwapchainWrapper {
    private:
    VulkanDevicesWrapper* mVulkanDevicesWrapper{};
    VkSwapchainKHR mSwapchainKHR{};
    VkSurfaceKHR mSurfaceKHR{};

    VkSwapchainCreateInfoKHR const* mSWAPCHAIN_KHR_CREATE_INFO{};

    void initSurface();
    void arise();

    static void checkHaveVkFormatColorspace(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE);
    static void checkHavePresentModeKHR(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE);

    public:
    VulkanSwapchainWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper);
    ~VulkanSwapchainWrapper();

    inline [[nodiscard]] VkSurfaceKHR getSurfaceKHR() noexcept { return mSurfaceKHR; }
    void setSwapchainKHRCreateInfoAndArise(VkSwapchainCreateInfoKHR const* GIVEN_SWAPCHAIN_KHR_CREATE_INFO);

    DELETE_COPY_CONSTRUCTORS(VulkanSwapchainWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanSwapchainWrapper)
};