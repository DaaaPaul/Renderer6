#include <iostream>
#include "Swapchain.hpp"

namespace Backend {
	Swapchain::Swapchain() :
		mpDevices{},
		mpSwapchainKHR{},
		mpSurfaceKHR{},
		mParameters{} {}

	Swapchain::Swapchain(Devices* givenDevices, SwapchainConstructInfo const& GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO) :
		mpDevices{ givenDevices },
		mpSwapchainKHR{},
		mpSurfaceKHR{ GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO.mpSurfaceKHR },
		mParameters{ GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO } {

		// reroute pointers
		mParameters.mSwapchainKHRCreateInfo.surface = mpSurfaceKHR;
		mParameters.mSwapchainKHRCreateInfo.pQueueFamilyIndices = &mParameters.mGRAPHICS_QUEUE_FAMILY_INDEX;

		// check that this gpu-surface pair supports the given format and present mode
		sCheckHaveVkFormatColorspace(*this, VkSurfaceFormatKHR(mParameters.mSwapchainKHRCreateInfo.imageFormat, mParameters.mSwapchainKHRCreateInfo.imageColorSpace));
		sCheckHavePresentModeKHR(*this, mParameters.mSwapchainKHRCreateInfo.presentMode);

		std::cout << "SET VULKAN SWAPCHAIN WRAPPER PARAMETERS:\n";
		std::cout << "\timages count: " << mParameters.mSwapchainKHRCreateInfo.minImageCount << "\n";
		std::cout << "\tformat: " << mParameters.mSwapchainKHRCreateInfo.imageFormat << "\n";
		std::cout << "\tcolorspace: " << mParameters.mSwapchainKHRCreateInfo.imageColorSpace << "\n";
		std::cout << "\textent: " << mParameters.mSwapchainKHRCreateInfo.imageExtent.width << "x" << mParameters.mSwapchainKHRCreateInfo.imageExtent.height << "\n";
		std::cout << "\timage usage: " << mParameters.mSwapchainKHRCreateInfo.imageUsage << "\n";
		std::cout << "\taccessed by queue family: " << mParameters.mSwapchainKHRCreateInfo.pQueueFamilyIndices[0] << "\n";
		std::cout << "\tpresent mode: " << mParameters.mSwapchainKHRCreateInfo.presentMode << "\n";

		std::cout << "Creating Swapchain...\n";

		// construct the swapchainKHR
		CHECK_VK_SUCCESS(
			vkCreateSwapchainKHR(mpDevices->mpLogicalDevice, &mParameters.mSwapchainKHRCreateInfo, nullptr, &mpSwapchainKHR),
			"Failed to create the swapchain"
		)
        
		std::cout << "Created Swapchain\n";
	}

	Swapchain::~Swapchain() {
		std::cout << "Destroying Swapchain...\n";

		vkDestroySwapchainKHR(mpDevices->mpLogicalDevice, mpSwapchainKHR, nullptr);
		vkDestroySurfaceKHR(mpDevices->mpBackend->mpInstance, mpSurfaceKHR, nullptr);

		std::cout << "Destroyed Swapchain\n";
	}

	void Swapchain::recreateThyself() {
		std::cout << "Recreating Swapchain...\n";

		vkDestroySwapchainKHR(mpDevices->mpLogicalDevice, mpSwapchainKHR, nullptr);
		vkDestroySurfaceKHR(mpDevices->mpBackend->mpInstance, mpSurfaceKHR, nullptr);

		// obtain updated construct info and put it into mParameters and mpSurfaceKHR
		SwapchainConstructInfo newConstructInfo(Swapchain::sGetConstructParameters(mpDevices->mpBackend->mpInstance, mpDevices->mpPhysicalDevice, mpDevices->mpBackend->mpWindow->mpGlfwWindow, mpDevices->mGRAPHICS_QUEUE_FAMILY_INDEX));
		mParameters.mpSurfaceKHR = newConstructInfo.mpSurfaceKHR;
		mParameters.mSwapchainKHRCreateInfo = newConstructInfo.mSwapchainKHRCreateInfo;
		mpSurfaceKHR = newConstructInfo.mpSurfaceKHR;

		// reroute updated construct info pointers
		mParameters.mSwapchainKHRCreateInfo.surface = mpSurfaceKHR;
		mParameters.mSwapchainKHRCreateInfo.pQueueFamilyIndices = &mParameters.mGRAPHICS_QUEUE_FAMILY_INDEX;

		CHECK_VK_SUCCESS(
			vkCreateSwapchainKHR(mpDevices->mpLogicalDevice, &mParameters.mSwapchainKHRCreateInfo, nullptr, &mpSwapchainKHR),
			"Failed to create the swapchain"
		)

		std::cout << "Recreated Swapchain\n";
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
		vkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.mpDevices->mpPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedVkFormatColorspacesCount, nullptr);
		std::vector<VkSurfaceFormatKHR> supportedVkFormatColorspaces(supportedVkFormatColorspacesCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(VULKAN_SWAPCHAIN_WRAPPER.mpDevices->mpPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedVkFormatColorspacesCount, supportedVkFormatColorspaces.data());

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
		vkGetPhysicalDeviceSurfacePresentModesKHR(VULKAN_SWAPCHAIN_WRAPPER.mpDevices->mpPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedPresentModeCount, nullptr);
		std::vector<VkPresentModeKHR> supportedVkFormatColorspaces(supportedPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(VULKAN_SWAPCHAIN_WRAPPER.mpDevices->mpPhysicalDevice, VULKAN_SWAPCHAIN_WRAPPER.mpSurfaceKHR, &supportedPresentModeCount, supportedVkFormatColorspaces.data());

		bool checkSuccess{ false };

		for(VkPresentModeKHR const& SUPPORTED : supportedVkFormatColorspaces) {
			if(SUPPORTED == CHECK_ME_PRESENT_MODE) {
				checkSuccess = true;
			}
		}

		CHECK_BOOL(checkSuccess, "Surface and physical device do not support desired present mode")
	}
}