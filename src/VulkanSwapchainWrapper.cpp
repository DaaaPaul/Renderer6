#include <iostream>
#include "VulkanSwapchainWrapper.hpp"

VulkanSwapchainWrapper::VulkanSwapchainWrapper(VkSwapchainCreateInfoKHR const* GIVEN_SWAPCHAIN_CREATE_INFO) :
    mSwapchainKHR{},
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

void VulkanSwapchainWrapper::checkHaveVkFormat(VulkanDevicesWrapper const& VULKAN_DEVICES_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE) {
    uint32_t supportedVkFormatColorspacesCount{};
    vkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_DEVICES_WRAPPER.mPhysicalDevice, VULKAN_DEVICES_WRAPPER.mVulkanBackendWrapper->mSurfaceKHR, &supportedVkFormatColorspacesCount, nullptr);
    std::vector<VkSurfaceFormatKHR> supportedVkFormatColorspaces(supportedVkFormatColorspacesCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_DEVICES_WRAPPER.mPhysicalDevice, VULKAN_DEVICES_WRAPPER.mVulkanBackendWrapper->mSurfaceKHR, &supportedVkFormatColorspacesCount, supportedVkFormatColorspaces.data());

    bool checkSuccess{ false };

    for(VkSurfaceFormatKHR const& SUPPORTED : supportedVkFormatColorspaces) {
        if(SUPPORTED.format == CHECK_ME_FORMAT_COLORSPACE.format && SUPPORTED.colorSpace == CHECK_ME_FORMAT_COLORSPACE.colorSpace) {
            checkSuccess = true;
        }
    }

    CHECK_BOOL(checkSuccess)
}

void VulkanSwapchainWrapper::checkHavePresentModeKHR(VulkanDevicesWrapper const& VULKAN_DEVICES_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE) {

}