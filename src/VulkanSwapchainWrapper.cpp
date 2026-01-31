#include <iostream>
#include "VulkanSwapchainWrapper.hpp"
#include "VulkanPFNs.h"

VulkanSwapchainWrapper::VulkanSwapchainWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper, VulkanSwapchainWrapperConstructInfo const& GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO) :
    mVulkanDevicesWrapper{ givenVulkanDevicesWrapper },
    mSwapchainKHR{},
    mSurfaceKHR{ GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO.mSURFACE_KHR },
    mParameters{ GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO } {

    // reroute pointers
    mParameters.mSwapchainKHRCreateInfo.surface = mSurfaceKHR;
    mParameters.mSwapchainKHRCreateInfo.pQueueFamilyIndices = &mParameters.mGRAPHICS_QUEUE_FAMILY_INDEX;

    // check that this gpu-surface pair supports the given format and present mode
    checkHaveVkFormatColorspace(*this, VkSurfaceFormatKHR(mParameters.mSwapchainKHRCreateInfo.imageFormat, mParameters.mSwapchainKHRCreateInfo.imageColorSpace));
    checkHavePresentModeKHR(*this, mParameters.mSwapchainKHRCreateInfo.presentMode);

    // construct the swapchainKHR
    CHECK_VK_SUCCESS(
    VulkanPFNs::gpVkCreateSwapchainKHR(mVulkanDevicesWrapper->mLogicalDevice, &mParameters.mSwapchainKHRCreateInfo, nullptr, &mSwapchainKHR),
    "Failed to create the swapchain"
    )
}

VulkanSwapchainWrapper::~VulkanSwapchainWrapper() {
    std::cout << "Destroying VulkanSwapchainWrapper...\n";

    VulkanPFNs::gpVkDestroySwapchainKHR(mVulkanDevicesWrapper->mLogicalDevice, mSwapchainKHR, nullptr);
    VulkanPFNs::gpVkDestroySurfaceKHR(mVulkanDevicesWrapper->mVulkanBackendWrapper->mInstance, mSurfaceKHR, nullptr);

    std::cout << "Destroyed VulkanSwapchainWrapper\n";
}

[[nodiscard]] VulkanSwapchainWrapper::VulkanSwapchainWrapperConstructInfo VulkanSwapchainWrapper::getConstructParameters(VkInstance instance, VkPhysicalDevice physicalDevice, GLFWwindow* window) {
    VkSurfaceKHR surfaceKHRToReturn{};
    CHECK_VK_SUCCESS(
        glfwCreateWindowSurface(instance, window, nullptr, &surfaceKHRToReturn),
        "Failed to create surface"
    )

    auto fGetWindowExtentInPixels = [physicalDevice, surfaceKHRToReturn, window]() -> VkExtent2D {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        CHECK_VK_SUCCESS(
            VulkanPFNs::gpVkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surfaceKHRToReturn, &surfaceCapabilities),
            "Failed to get physical device surface capabilities"
        )

        VkSwapchainCreateInfoKHR mSwapchainKHRCreateInfo{};
        VkExtent2D surfaceExtentInPixels{};
        if (surfaceCapabilities.currentExtent.width == UINT32_MAX && surfaceCapabilities.currentExtent.height == UINT32_MAX) {
            glfwGetFramebufferSize(window, reinterpret_cast<int*>(&surfaceExtentInPixels.width), reinterpret_cast<int*>(&surfaceExtentInPixels.height));
        } else {
            surfaceExtentInPixels = VkExtent2D(surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
        }

        return surfaceExtentInPixels;
    };

    auto fGetGraphicsQueueFamilyIndex = [physicalDevice]() -> uint32_t {
        uint32_t physicalDeviceQueueFamilyCount{};
        VulkanPFNs::gpVkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &physicalDeviceQueueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> physicalDeviceQueueFamilyProperties(physicalDeviceQueueFamilyCount);
        VulkanPFNs::gpVkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &physicalDeviceQueueFamilyCount, physicalDeviceQueueFamilyProperties.data());

        uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
        for (int i = 0; i < physicalDeviceQueueFamilyCount; i++) {
            if (physicalDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsQueueFamilyIndex = i;
            }
        }

        if (graphicsQueueFamilyIndex == UINT32_MAX) {
            throw std::runtime_error("Did not find a graphics queue for physical device");
        } else {
            return graphicsQueueFamilyIndex;
        }
    };
    const uint32_t GRAPHICS_QUEUE_FAMILY_INDEX = fGetGraphicsQueueFamilyIndex();
    
    VkSwapchainCreateInfoKHR swapchainKHRCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = nullptr, // reroute needed
        .minImageCount = 4,
        .imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
        .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = fGetWindowExtentInPixels(),
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = nullptr, // reroute needed
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    return VulkanSwapchainWrapperConstructInfo(surfaceKHRToReturn, swapchainKHRCreateInfo, GRAPHICS_QUEUE_FAMILY_INDEX);
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