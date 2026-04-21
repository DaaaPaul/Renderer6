#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

namespace Engine {
	namespace Swapchain {
		inline VkSwapchainKHR gSwapchain{};
		inline std::vector<VkImage> gImages{};
		inline VkSurfaceKHR gSurface{};
		inline VkSwapchainCreateInfoKHR gStatus{};

		inline VkExtent2D gImageSize{};
		inline constexpr uint32_t gIMAGE_COUNT = 4;
		inline constexpr VkFormat gIMAGE_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;
		inline constexpr VkColorSpaceKHR gIMAGE_COLOR_SPACE = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		inline constexpr VkPresentModeKHR gPRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

		void init();
		void deInit();
		void recreate();

		void createSurface();
		void populateCurrentSwapchainStatus();
		void createSwapchain();
		void populateImages();
		void destroySurface();
		void destroySwapchain();

		void populateImageSize();
		void checkImageFormatAndColorspaceSupported();
		void checkPresentModeSupported();
	}
}