#include <iostream>
#include "Global.h"
#include "Swapchain.hpp"

namespace Backend {
	[[nodiscard]] VkExtent2D Swapchain::getCurrentExtent() const noexcept {
		const VkSurfaceCapabilitiesKHR SURFACE_CAPABILITIES(
			[this]() -> VkSurfaceCapabilitiesKHR {
				VkSurfaceCapabilitiesKHR capabilities{};
				CHECK_VK_SUCCESS(
					vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Global::getDevices().getPhysicalDevice(), pSurface, &capabilities),
					"Failed to get physical device surface capabilities"
				)
				return capabilities;
			}()
		);

		VkExtent2D surfaceExtentInPixels{};
		if (SURFACE_CAPABILITIES.currentExtent.width == UINT32_MAX && SURFACE_CAPABILITIES.currentExtent.height == UINT32_MAX) {
			glfwGetFramebufferSize(Global::getWindow().getGlfwWindow(), reinterpret_cast<int*>(&surfaceExtentInPixels.width), reinterpret_cast<int*>(&surfaceExtentInPixels.height));
		} else {
			surfaceExtentInPixels = VkExtent2D(SURFACE_CAPABILITIES.currentExtent.width, SURFACE_CAPABILITIES.currentExtent.height);
		}

		return surfaceExtentInPixels;
	}

	std::vector<VkImage> Swapchain::getImages() const noexcept {
		uint32_t imageCount = UINT32_MAX;
		vkGetSwapchainImagesKHR(pDevices->getLogicalDevice(), pSwapchain, &imageCount, nullptr);
		std::vector<VkImage> images(imageCount, {});
		vkGetSwapchainImagesKHR(pDevices->getLogicalDevice(), pSwapchain, &imageCount, images.data());

		return images;
	}

	void Swapchain::recreate() {
		while(getCurrentExtent().width == 0 && getCurrentExtent().height == 0) {
			glfwWaitEvents(); // do not process anything when window is minimized
		}

		vkDestroySwapchainKHR(pDevices->getLogicalDevice(), pSwapchain, nullptr);
		const VkExtent2D SURFACE_EXTENT(getCurrentExtent());
		const std::vector<uint32_t> ACCESSOR_GFXQF{ Global::getDevices().getGraphicsQfIndex() };
    
		const VkSwapchainCreateInfoKHR SWAPCHAIN_INFO{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = pSurface,
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
			vkCreateSwapchainKHR(pDevices->getLogicalDevice(), &SWAPCHAIN_INFO, nullptr, &pSwapchain),
			"Failed to create the swapchain"
		)
	}

	void Swapchain::checkHaveFormatColorspace(Swapchain const& SWAPCHAIN, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE) {
		uint32_t supportedVkFormatColorspacesCount{};
		vkGetPhysicalDeviceSurfaceFormatsKHR(SWAPCHAIN.pDevices->getPhysicalDevice(), SWAPCHAIN.pSurface, &supportedVkFormatColorspacesCount, nullptr);
		std::vector<VkSurfaceFormatKHR> supportedVkFormatColorspaces(supportedVkFormatColorspacesCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(SWAPCHAIN.pDevices->getPhysicalDevice(), SWAPCHAIN.pSurface, &supportedVkFormatColorspacesCount, supportedVkFormatColorspaces.data());

		bool checkSuccess{ false };

		for (VkSurfaceFormatKHR const& SUPPORTED : supportedVkFormatColorspaces) {
			if (SUPPORTED.format == CHECK_ME_FORMAT_COLORSPACE.format && SUPPORTED.colorSpace == CHECK_ME_FORMAT_COLORSPACE.colorSpace) {
				checkSuccess = true;
			}
		}

		CHECK_BOOL(checkSuccess, "Surface and physical device do not support desired format")
	}

	void Swapchain::checkHavePresentModeKHR(Swapchain const& SWAPCHAIN, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE) {
		uint32_t supportedPresentModeCount{};
		vkGetPhysicalDeviceSurfacePresentModesKHR(SWAPCHAIN.pDevices->getPhysicalDevice(), SWAPCHAIN.pSurface, &supportedPresentModeCount, nullptr);
		std::vector<VkPresentModeKHR> supportedVkFormatColorspaces(supportedPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(SWAPCHAIN.pDevices->getPhysicalDevice(), SWAPCHAIN.pSurface, &supportedPresentModeCount, supportedVkFormatColorspaces.data());

		bool checkSuccess{ false };

		for(VkPresentModeKHR const& SUPPORTED : supportedVkFormatColorspaces) {
			if(SUPPORTED == CHECK_ME_PRESENT_MODE) {
				checkSuccess = true;
			}
		}

		CHECK_BOOL(checkSuccess, "Surface and physical device do not support desired present mode")
	}

	Swapchain::Swapchain(Devices* pGivenDevices, CreateInfo&& salvageCreateInfo) :
		pDevices{ pGivenDevices },
		pSwapchain{},
		pSurface{ salvageCreateInfo.pSurface },
		CREATE_INFO{ std::move(salvageCreateInfo) } {

		checkHaveFormatColorspace(*this, VkSurfaceFormatKHR(CREATE_INFO.createInfo.imageFormat, CREATE_INFO.createInfo.imageColorSpace));
		checkHavePresentModeKHR(*this, CREATE_INFO.createInfo.presentMode);

		CHECK_VK_SUCCESS(
			vkCreateSwapchainKHR(pDevices->getLogicalDevice(), &CREATE_INFO.createInfo, nullptr, &pSwapchain),
			"Failed to create the swapchain"
		)
	}

	Swapchain::~Swapchain() {
		vkDestroySwapchainKHR(pDevices->getLogicalDevice(), pSwapchain, nullptr);
		vkDestroySurfaceKHR(pDevices->getInstance()->getInstance(), pSurface, nullptr);
	}
}