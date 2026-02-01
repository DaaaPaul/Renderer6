#include <iostream>
#include "VulkanSwapchainWrapper.hpp"
#include "VulkanPFNs.h"

VulkanSwapchainWrapper::VulkanSwapchainWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper, VulkanSwapchainWrapperConstructInfo const& GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO) :
    mpVulkanDevicesWrapper{ givenVulkanDevicesWrapper },
    mpSwapchainKHR{},
    mpSurfaceKHR{ GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO.mSURFACE_KHR },
    mpImageViews{},
    mParameters{ GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO } {

    // reroute pointers
    mParameters.mSwapchainKHRCreateInfo.surface = mpSurfaceKHR;
    mParameters.mSwapchainKHRCreateInfo.pQueueFamilyIndices = &mParameters.mGRAPHICS_QUEUE_FAMILY_INDEX;

    // check that this gpu-surface pair supports the given format and present mode
    checkHaveVkFormatColorspace(*this, VkSurfaceFormatKHR(mParameters.mSwapchainKHRCreateInfo.imageFormat, mParameters.mSwapchainKHRCreateInfo.imageColorSpace));
    checkHavePresentModeKHR(*this, mParameters.mSwapchainKHRCreateInfo.presentMode);

    std::cout << "SET VULKAN SWAPCHAIN WRAPPER PARAMETERS:\n";
    std::cout << "\timages count: " << mParameters.mSwapchainKHRCreateInfo.minImageCount << "\n";
    std::cout << "\tformat: " << mParameters.mSwapchainKHRCreateInfo.imageFormat << "\n";
    std::cout << "\tcolorspace: " << mParameters.mSwapchainKHRCreateInfo.imageColorSpace << "\n";
    std::cout << "\textent: " << mParameters.mSwapchainKHRCreateInfo.imageExtent.width << "x" << mParameters.mSwapchainKHRCreateInfo.imageExtent.height << "\n";
    std::cout << "\timage usage: " << mParameters.mSwapchainKHRCreateInfo.imageUsage << "\n";
    std::cout << "\taccessed by queue family: " << mParameters.mSwapchainKHRCreateInfo.pQueueFamilyIndices[0] << "\n";
    std::cout << "\tpresent mode: " << mParameters.mSwapchainKHRCreateInfo.presentMode << "\n";

    std::cout << "Creating VulkanSwapchainWrapper...\n";

    // construct the swapchainKHR
    CHECK_VK_SUCCESS(
        VulkanPFNs::gpVkCreateSwapchainKHR(mpVulkanDevicesWrapper->mpLogicalDevice, &mParameters.mSwapchainKHRCreateInfo, nullptr, &mpSwapchainKHR),
        "Failed to create the swapchain"
    )

    auto createImageViews = [this]() -> void {
        uint32_t swapchainImageCount{};
		VulkanPFNs::gpVkGetSwapchainImagesKHR(this->mpVulkanDevicesWrapper->mpLogicalDevice, this->mpSwapchainKHR, &swapchainImageCount, nullptr);
		std::vector<VkImage> swapchainImages(swapchainImageCount);
		VulkanPFNs::gpVkGetSwapchainImagesKHR(this->mpVulkanDevicesWrapper->mpLogicalDevice, this->mpSwapchainKHR, &swapchainImageCount, swapchainImages.data());

		VkImageViewCreateInfo rollingImageViewCreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = VK_NULL_HANDLE,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = VK_FORMAT_R8G8B8A8_SRGB,
			.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
		};
		this->mpImageViews.resize(swapchainImageCount, VK_NULL_HANDLE);
		for(int i = 0; i < swapchainImageCount; i++) {
			rollingImageViewCreateInfo.image = swapchainImages[i];
			VulkanPFNs::gpVkCreateImageView(this->mpVulkanDevicesWrapper->mpLogicalDevice, &rollingImageViewCreateInfo, nullptr, &(this->mpImageViews[i]));
		}
	};
	createImageViews();
        
    std::cout << "Created VulkanSwapchainWrapper\n";
}

VulkanSwapchainWrapper::~VulkanSwapchainWrapper() {
    std::cout << "Destroying VulkanSwapchainWrapper...\n";

	for(VkImageView& imageView : mpImageViews) {
		VulkanPFNs::gpVkDestroyImageView(mpVulkanDevicesWrapper->mpLogicalDevice, imageView, nullptr);
	}
    VulkanPFNs::gpVkDestroySwapchainKHR(mpVulkanDevicesWrapper->mpLogicalDevice, mpSwapchainKHR, nullptr);
    VulkanPFNs::gpVkDestroySurfaceKHR(mpVulkanDevicesWrapper->mpVulkanBackendWrapper->mpInstance, mpSurfaceKHR, nullptr);

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
    VulkanPFNs::gpVkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.mpVulkanDevicesWrapper->mpPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedVkFormatColorspacesCount, nullptr);
    std::vector<VkSurfaceFormatKHR> supportedVkFormatColorspaces(supportedVkFormatColorspacesCount);
    VulkanPFNs::gpVkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.mpVulkanDevicesWrapper->mpPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedVkFormatColorspacesCount, supportedVkFormatColorspaces.data());

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
    VulkanPFNs::gpVkGetPhysicalDeviceSurfacePresentModesKHR(VULKAN_SWAPCHAIN_WRAPPER.mpVulkanDevicesWrapper->mpPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedPresentModeCount, nullptr);
    std::vector<VkPresentModeKHR> supportedVkFormatColorspaces(supportedPresentModeCount);
    VulkanPFNs::gpVkGetPhysicalDeviceSurfacePresentModesKHR(VULKAN_SWAPCHAIN_WRAPPER.mpVulkanDevicesWrapper->mpPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedPresentModeCount, supportedVkFormatColorspaces.data());

    bool checkSuccess{ false };

    for(VkPresentModeKHR const& SUPPORTED : supportedVkFormatColorspaces) {
        if(SUPPORTED == CHECK_ME_PRESENT_MODE) {
            checkSuccess = true;
        }
    }

    CHECK_BOOL(checkSuccess, "Surface and physical device do not support desired present mode")
}