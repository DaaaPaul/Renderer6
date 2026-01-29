#include <iostream>
#include "VulkanSwapchainWrapper.hpp"
#include "VulkanPFNs.h"

VulkanSwapchainWrapper::VulkanSwapchainWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper, VkSwapchainCreateInfoKHR const* GIVEN_SWAPCHAIN_CREATE_INFO) :
    mVulkanDevicesWrapper{ givenVulkanDevicesWrapper },
    mSwapchainKHR{},
    mSurfaceKHR{},
    mSWAPCHAIN_KHR_CREATE_INFO{ GIVEN_SWAPCHAIN_CREATE_INFO } {

    
}

VulkanSwapchainWrapper::~VulkanSwapchainWrapper() {
    std::cout << "Destroying VulkanSwapchainWrapper...\n";

    std::cout << "Destroyed VulkanSwapchainWrapper\n";

}

void VulkanSwapchainWrapper::arise() {
    std::cout << "Creating VulkanSwapchainWrapper...\n";

    std::cout << "Created VulkanSwapchainWrapper\n";
}

void VulkanSwapchainWrapper::checkHaveVkFormat(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE) {
    uint32_t supportedVkFormatColorspacesCount{};
    VulkanPFNs::gpVkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.mVulkanDevicesWrapper->mPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mSurfaceKHR, &supportedVkFormatColorspacesCount, nullptr);
    std::vector<VkSurfaceFormatKHR> supportedVkFormatColorspaces(supportedVkFormatColorspacesCount);
    VulkanPFNs::gpVkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.mVulkanDevicesWrapper->mPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mSurfaceKHR, &supportedVkFormatColorspacesCount, supportedVkFormatColorspaces.data());

    bool checkSuccess{ false };

    for(VkSurfaceFormatKHR const& SUPPORTED : supportedVkFormatColorspaces) {
        if(SUPPORTED.format == CHECK_ME_FORMAT_COLORSPACE.format && SUPPORTED.colorSpace == CHECK_ME_FORMAT_COLORSPACE.colorSpace) {
            checkSuccess = true;
        }
    }

    CHECK_BOOL(checkSuccess)
}

void VulkanSwapchainWrapper::checkHavePresentModeKHR(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE) {
    
}