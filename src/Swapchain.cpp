#include <iostream>
#include "GlobalState.h"
#include "Swapchain.hpp"

namespace Backend {
	[[nodiscard]] VkExtent2D Swapchain::getCurrentExtent() const noexcept {
		const VkSurfaceCapabilitiesKHR SURFACE_CAPABILITIES(
			[this]() -> VkSurfaceCapabilitiesKHR {
				VkSurfaceCapabilitiesKHR capabilities{};
				CHECK_VK_SUCCESS(
					vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GlobalState::Core::getDevices().getPhysicalDevice(), surface, &capabilities),
					"Failed to get physical device surface capabilities"
				)
				return capabilities;
			}()
		);

		VkExtent2D surfaceExtentInPixels{};
		if (SURFACE_CAPABILITIES.currentExtent.width == UINT32_MAX && SURFACE_CAPABILITIES.currentExtent.height == UINT32_MAX) {
			glfwGetFramebufferSize(GlobalState::Core::getWindow().getGlfwWindow(), reinterpret_cast<int*>(&surfaceExtentInPixels.width), reinterpret_cast<int*>(&surfaceExtentInPixels.height));
		} else {
			surfaceExtentInPixels = VkExtent2D(SURFACE_CAPABILITIES.currentExtent.width, SURFACE_CAPABILITIES.currentExtent.height);
		}

		return surfaceExtentInPixels;
	}

	void Swapchain::recreateThyself() {
		while(getCurrentExtent().width == 0 && getCurrentExtent().height == 0) {
			glfwWaitEvents(); // do not process anything when window is minimized
		}

		vkDestroySwapchainKHR(devices->getLogicalDevice(), swapchain, nullptr);
		const VkExtent2D SURFACE_EXTENT(getCurrentExtent());
		const std::vector<uint32_t> ACCESSOR_GFXQF{ GlobalState::Core::getDevices().getGraphicsQfIndex() };
    
		const VkSwapchainCreateInfoKHR SWAPCHAIN_INFO{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = surface,
			.minImageCount = 4,
			.imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
			.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
			.imageExtent = SURFACE_EXTENT,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = static_cast<uint32_t>(ACCESSOR_GFXQF.size()),
			.pQueueFamilyIndices = ACCESSOR_GFXQF.data(),
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
			.clipped = VK_TRUE,
		};

		CHECK_VK_SUCCESS(
			vkCreateSwapchainKHR(devices->getLogicalDevice(), &SWAPCHAIN_INFO, nullptr, &swapchain),
			"Failed to create the swapchain"
		)
	}

	void Swapchain::checkHaveFormatColorspace(Swapchain const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE) {
		uint32_t supportedVkFormatColorspacesCount{};
		vkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.devices->getPhysicalDevice(), VULKAN_SWAPCHAIN_WRAPPER.surface, &supportedVkFormatColorspacesCount, nullptr);
		std::vector<VkSurfaceFormatKHR> supportedVkFormatColorspaces(supportedVkFormatColorspacesCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.devices->getPhysicalDevice(), VULKAN_SWAPCHAIN_WRAPPER.surface, &supportedVkFormatColorspacesCount, supportedVkFormatColorspaces.data());

		bool checkSuccess{ false };

		for (VkSurfaceFormatKHR const& SUPPORTED : supportedVkFormatColorspaces) {
			if (SUPPORTED.format == CHECK_ME_FORMAT_COLORSPACE.format && SUPPORTED.colorSpace == CHECK_ME_FORMAT_COLORSPACE.colorSpace) {
				checkSuccess = true;
			}
		}

		CHECK_BOOL(checkSuccess, "Surface and physical device do not support desired format")
	}

	void Swapchain::checkHavePresentModeKHR(Swapchain const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE) {
		uint32_t supportedPresentModeCount{};
		vkGetPhysicalDeviceSurfacePresentModesKHR(VULKAN_SWAPCHAIN_WRAPPER.devices->getPhysicalDevice(), VULKAN_SWAPCHAIN_WRAPPER.surface, &supportedPresentModeCount, nullptr);
		std::vector<VkPresentModeKHR> supportedVkFormatColorspaces(supportedPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(VULKAN_SWAPCHAIN_WRAPPER.devices->getPhysicalDevice(), VULKAN_SWAPCHAIN_WRAPPER.surface, &supportedPresentModeCount, supportedVkFormatColorspaces.data());

		bool checkSuccess{ false };

		for(VkPresentModeKHR const& SUPPORTED : supportedVkFormatColorspaces) {
			if(SUPPORTED == CHECK_ME_PRESENT_MODE) {
				checkSuccess = true;
			}
		}

		CHECK_BOOL(checkSuccess, "Surface and physical device do not support desired present mode")
	}

	Swapchain::Swapchain(Devices* givenDevices, CreateInfo&& salvageCreateInfo) :
		devices{ givenDevices },
		swapchain{},
		surface{ salvageCreateInfo.surface },
		CREATE_INFO{ std::move(salvageCreateInfo) } {

		// check that this gpu-surface pair supports the given format and present mode
		checkHaveFormatColorspace(*this, VkSurfaceFormatKHR(CREATE_INFO.createInfo.imageFormat, CREATE_INFO.createInfo.imageColorSpace));
		checkHavePresentModeKHR(*this, CREATE_INFO.createInfo.presentMode);

		// construct the swapchainKHR
		CHECK_VK_SUCCESS(
			vkCreateSwapchainKHR(devices->getLogicalDevice(), &CREATE_INFO.createInfo, nullptr, &swapchain),
			"Failed to create the swapchain"
		)
	}

	Swapchain::~Swapchain() {
		vkDestroySwapchainKHR(devices->getLogicalDevice(), swapchain, nullptr);
		vkDestroySurfaceKHR(devices->getInstance()->getInstance(), surface, nullptr);
	}
}