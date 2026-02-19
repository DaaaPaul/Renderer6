#include <iostream>
#include "Swapchain.hpp"

namespace Backend {
	Swapchain::Swapchain() :
		devices{},
		mpSwapchainKHR{},
		mpSurfaceKHR{},
		CREATE_INFO{} {}

	Swapchain::Swapchain(Devices* givenDevices, SwapchainConstructInfo const& GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO) :
		devices{ givenDevices },
		mpSwapchainKHR{},
		mpSurfaceKHR{ GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO.mpSurfaceKHR },
		CREATE_INFO{ GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO } {

		// reroute pointers
		CREATE_INFO.mSwapchainKHRCreateInfo.surface = mpSurfaceKHR;
		CREATE_INFO.mSwapchainKHRCreateInfo.pQueueFamilyIndices = &CREATE_INFO.GRAPHICS_QF_INDEX;

		// check that this gpu-surface pair supports the given format and present mode
		sCheckHaveVkFormatColorspace(*this, VkSurfaceFormatKHR(CREATE_INFO.mSwapchainKHRCreateInfo.imageFormat, CREATE_INFO.mSwapchainKHRCreateInfo.imageColorSpace));
		sCheckHavePresentModeKHR(*this, CREATE_INFO.mSwapchainKHRCreateInfo.presentMode);

		// construct the swapchainKHR
		CHECK_VK_SUCCESS(
			vkCreateSwapchainKHR(devices->logicalDevice, &CREATE_INFO.mSwapchainKHRCreateInfo, nullptr, &mpSwapchainKHR),
			"Failed to create the swapchain"
		)
	}

	Swapchain::~Swapchain() {
		vkDestroySwapchainKHR(devices->logicalDevice, mpSwapchainKHR, nullptr);
		vkDestroySurfaceKHR(devices->instance->instance, mpSurfaceKHR, nullptr);
	}

	void Swapchain::recreateThyself() {
		vkDestroySwapchainKHR(devices->logicalDevice, mpSwapchainKHR, nullptr);
		vkDestroySurfaceKHR(devices->instance->instance, mpSurfaceKHR, nullptr);

		// obtain updated construct info and put it into createInfo and mpSurfaceKHR
		SwapchainConstructInfo newConstructInfo(Swapchain::sGetConstructParameters(devices->instance->instance, devices->physicalDevice, devices->instance->WINDOW->glfwWindow, devices->GRAPHICS_QF_INDEX));
		CREATE_INFO.mpSurfaceKHR = newConstructInfo.mpSurfaceKHR;
		CREATE_INFO.mSwapchainKHRCreateInfo = newConstructInfo.mSwapchainKHRCreateInfo;
		mpSurfaceKHR = newConstructInfo.mpSurfaceKHR;

		// reroute updated construct info pointers
		CREATE_INFO.mSwapchainKHRCreateInfo.surface = mpSurfaceKHR;
		CREATE_INFO.mSwapchainKHRCreateInfo.pQueueFamilyIndices = &CREATE_INFO.GRAPHICS_QF_INDEX;

		CHECK_VK_SUCCESS(
			vkCreateSwapchainKHR(devices->logicalDevice, &CREATE_INFO.mSwapchainKHRCreateInfo, nullptr, &mpSwapchainKHR),
			"Failed to create the swapchain"
		)
	}

	[[nodiscard]] Swapchain::SwapchainConstructInfo Swapchain::sGetConstructParameters(VkInstance pInstance, VkPhysicalDevice pPhysicalDevice, GLFWwindow* pGlfwWindow, uint32_t const& GRAPHICS_QUEUE_FAMILY_INDEX) {
		VkSurfaceKHR surfaceKHRToReturn{};
		CHECK_VK_SUCCESS(
			glfwCreateWindowSurface(pInstance, pGlfwWindow, nullptr, &surfaceKHRToReturn),
			"Failed to create surface"
		)

		auto fGetWindowExtentInPixels = [pPhysicalDevice, surfaceKHRToReturn, pGlfwWindow]() -> VkExtent2D {
			VkSurfaceCapabilitiesKHR surfaceCapabilities{};
			CHECK_VK_SUCCESS(
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pPhysicalDevice, surfaceKHRToReturn, &surfaceCapabilities),
				"Failed to get physical device surface capabilities"
			)

			VkSwapchainCreateInfoKHR mSwapchainKHRCreateInfo{};
			VkExtent2D surfaceExtentInPixels{};
			if (surfaceCapabilities.currentExtent.width == UINT32_MAX && surfaceCapabilities.currentExtent.height == UINT32_MAX) {
				glfwGetFramebufferSize(pGlfwWindow, reinterpret_cast<int*>(&surfaceExtentInPixels.width), reinterpret_cast<int*>(&surfaceExtentInPixels.height));
			} else {
				surfaceExtentInPixels = VkExtent2D(surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
			}

			return surfaceExtentInPixels;
		};

		const uint32_t RETURN_GRAPHICS_QUEUE_FAMILY_INDEX = GRAPHICS_QUEUE_FAMILY_INDEX;
    
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

		return SwapchainConstructInfo(surfaceKHRToReturn, swapchainKHRCreateInfo, RETURN_GRAPHICS_QUEUE_FAMILY_INDEX);
	}

	void Swapchain::sCheckHaveVkFormatColorspace(Swapchain const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE) {
		uint32_t supportedVkFormatColorspacesCount{};
		vkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.devices->physicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedVkFormatColorspacesCount, nullptr);
		std::vector<VkSurfaceFormatKHR> supportedVkFormatColorspaces(supportedVkFormatColorspacesCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.devices->physicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedVkFormatColorspacesCount, supportedVkFormatColorspaces.data());

		bool checkSuccess{ false };

		for (VkSurfaceFormatKHR const& SUPPORTED : supportedVkFormatColorspaces) {
			if (SUPPORTED.format == CHECK_ME_FORMAT_COLORSPACE.format && SUPPORTED.colorSpace == CHECK_ME_FORMAT_COLORSPACE.colorSpace) {
				checkSuccess = true;
			}
		}

		CHECK_BOOL(checkSuccess, "Surface and physical device do not support desired format")
	}

	void Swapchain::sCheckHavePresentModeKHR(Swapchain const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE) {
		uint32_t supportedPresentModeCount{};
		vkGetPhysicalDeviceSurfacePresentModesKHR(VULKAN_SWAPCHAIN_WRAPPER.devices->physicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedPresentModeCount, nullptr);
		std::vector<VkPresentModeKHR> supportedVkFormatColorspaces(supportedPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(VULKAN_SWAPCHAIN_WRAPPER.devices->physicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedPresentModeCount, supportedVkFormatColorspaces.data());

		bool checkSuccess{ false };

		for(VkPresentModeKHR const& SUPPORTED : supportedVkFormatColorspaces) {
			if(SUPPORTED == CHECK_ME_PRESENT_MODE) {
				checkSuccess = true;
			}
		}

		CHECK_BOOL(checkSuccess, "Surface and physical device do not support desired present mode")
	}
}