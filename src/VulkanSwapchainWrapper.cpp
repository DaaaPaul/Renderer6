#include <iostream>
#include "VulkanSwapchainWrapper.hpp"
#include "VulkanPFNs.h"

VulkanSwapchainWrapper::VulkanSwapchainWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper) :
    mVulkanDevicesWrapper{ givenVulkanDevicesWrapper },
    mSwapchainKHR{},
    mSurfaceKHR{},
    mSWAPCHAIN_KHR_CREATE_INFO{} {

    initSurface();
}

VulkanSwapchainWrapper::~VulkanSwapchainWrapper() {
    std::cout << "Destroying VulkanSwapchainWrapper...\n";

    VulkanPFNs::gpVkDestroySwapchainKHR(mVulkanDevicesWrapper->mLogicalDevice, mSwapchainKHR, nullptr);
    VulkanPFNs::gpVkDestroySurfaceKHR(mVulkanDevicesWrapper->mVulkanBackendWrapper->mInstance, mSurfaceKHR, nullptr);

    std::cout << "Destroyed VulkanSwapchainWrapper\n";
}

void VulkanSwapchainWrapper::setSwapchainKHRCreateInfoAndArise(VkSwapchainCreateInfoKHR const* GIVEN_SWAPCHAIN_KHR_CREATE_INFO) {
    mSWAPCHAIN_KHR_CREATE_INFO = GIVEN_SWAPCHAIN_KHR_CREATE_INFO;
    
    VulkanSwapchainWrapper::checkHaveVkFormatColorspace(*this, VkSurfaceFormatKHR(mSWAPCHAIN_KHR_CREATE_INFO->imageFormat, mSWAPCHAIN_KHR_CREATE_INFO->imageColorSpace));
    VulkanSwapchainWrapper::checkHavePresentModeKHR(*this, mSWAPCHAIN_KHR_CREATE_INFO->presentMode);
    arise();
}

void VulkanSwapchainWrapper::initSurface() {
    CHECK_VK_SUCCESS(
    glfwCreateWindowSurface(mVulkanDevicesWrapper->mVulkanBackendWrapper->mInstance, mVulkanDevicesWrapper->mVulkanBackendWrapper->mGlfwWindowWrapper->mGlfwWindow, nullptr, &mSurfaceKHR),
    "Failed to create surface"
    )
}

void VulkanSwapchainWrapper::arise() {
    std::cout << "Creating VulkanSwapchainWrapper...\n";

    CHECK_VK_SUCCESS(
    VulkanPFNs::gpVkCreateSwapchainKHR(mVulkanDevicesWrapper->mLogicalDevice, mSWAPCHAIN_KHR_CREATE_INFO, nullptr, &mSwapchainKHR),
    "Failed to create swapchain"
    )

    std::cout << "Created VulkanSwapchainWrapper\n";
}

void VulkanSwapchainWrapper::checkHaveVkFormatColorspace(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE) {
    uint32_t supportedVkFormatColorspacesCount{};
    VulkanPFNs::gpVkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.mVulkanDevicesWrapper->mPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mSurfaceKHR, &supportedVkFormatColorspacesCount, nullptr);
    std::vector<VkSurfaceFormatKHR> supportedVkFormatColorspaces(supportedVkFormatColorspacesCount);
    VulkanPFNs::gpVkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.mVulkanDevicesWrapper->mPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mSurfaceKHR, &supportedVkFormatColorspacesCount, supportedVkFormatColorspaces.data());

    bool checkSuccess{ false };

    for (VkSurfaceFormatKHR const& SUPPORTED : supportedVkFormatColorspaces) {
        if (SUPPORTED.format == CHECK_ME_FORMAT_COLORSPACE.format && SUPPORTED.colorSpace == CHECK_ME_FORMAT_COLORSPACE.colorSpace) {
            checkSuccess = true;
        }
    }

    CHECK_BOOL(checkSuccess, "Surface and physical device do not support desired format")
}

void VulkanSwapchainWrapper::checkHavePresentModeKHR(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE) {
    uint32_t supportedPresentModeCount{};
    VulkanPFNs::gpVkGetPhysicalDeviceSurfacePresentModesKHR(VULKAN_SWAPCHAIN_WRAPPER.mVulkanDevicesWrapper->mPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mSurfaceKHR, &supportedPresentModeCount, nullptr);
    std::vector<VkPresentModeKHR> supportedVkFormatColorspaces(supportedPresentModeCount);
    VulkanPFNs::gpVkGetPhysicalDeviceSurfacePresentModesKHR(VULKAN_SWAPCHAIN_WRAPPER.mVulkanDevicesWrapper->mPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mSurfaceKHR, &supportedPresentModeCount, supportedVkFormatColorspaces.data());

    bool checkSuccess{ false };

    for(VkPresentModeKHR const& SUPPORTED : supportedVkFormatColorspaces) {
        if(SUPPORTED == CHECK_ME_PRESENT_MODE) {
            checkSuccess = true;
        }
    }

    CHECK_BOOL(checkSuccess, "Surface and physical device do not support desired present mode")
}