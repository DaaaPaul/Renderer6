#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

namespace Swapchain {
	inline VkSwapchainKHR g_swapchain{};
	inline std::vector<VkImage> g_images{};
	inline std::vector<VkImageView> g_image_views{};
	inline VkSurfaceKHR g_surface{};
	inline VkSwapchainCreateInfoKHR g_status{};

	inline VkExtent2D g_image_extent{};
	inline constexpr uint32_t g_IMAGE_COUNT = 4;
	inline constexpr VkFormat g_IMAGE_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;
	inline constexpr VkColorSpaceKHR g_IMAGE_COLOR_SPACE = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	inline constexpr VkPresentModeKHR g_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

	void init();
	void destroy();
	void recreate();

	VkSwapchainKHR create_swapchain();

	VkExtent2D get_image_extent();
	void check_format_colorspace(VkFormat format, VkColorSpaceKHR colorspace);
	void check_present_mode(VkPresentModeKHR present_mode);
}
