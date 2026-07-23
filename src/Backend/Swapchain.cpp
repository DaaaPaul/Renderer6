#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Backend/Swapchain.h"
#include "Backend/LogicalDevice.h"
#include "Backend/PhysicalDevice.h"
#include "Backend/Instance.h"
#include "Backend/Window.h"
#include "Utility/Vulkan.h"

namespace Swapchain {
	void init() {
		g_surface = Vulkan::create_surface();

		g_image_extent = get_image_extent();
		check_format_colorspace(g_IMAGE_FORMAT, g_IMAGE_COLOR_SPACE);
		check_present_mode(g_PRESENT_MODE);

		g_swapchain = create_swapchain();
		g_images = Vulkan::get_swapchain_images(g_swapchain);
		g_image_views = Vulkan::get_image_views(g_images, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, g_IMAGE_FORMAT);
	}

	void destroy() {
		vkDestroySwapchainKHR(g_device, g_swapchain, nullptr);
		vkDestroySurfaceKHR(Instance::g_instance, g_surface, nullptr);

		for(VkImageView view : g_image_views) {
			vkDestroyImageView(g_device, view, nullptr);
		}
	}

	void recreate() {
		g_image_extent = get_image_extent();
		while(g_image_extent.width == 0 && g_image_extent.height == 0) {
			glfwWaitEvents(); // wait for the next glfw event, and process it when it comes
			g_image_extent = get_image_extent(); // query window size after glfw event is processed
		}

		vkDestroySwapchainKHR(g_device, g_swapchain, nullptr);
		g_swapchain = create_swapchain();
		g_images = Vulkan::get_swapchain_images(g_swapchain);
	}

	VkSwapchainKHR create_swapchain() {
		VkSwapchainKHR swapchain{};

		g_status = VkSwapchainCreateInfoKHR{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = g_surface,
			.minImageCount = g_IMAGE_COUNT,
			.imageFormat = g_IMAGE_FORMAT,
			.imageColorSpace = g_IMAGE_COLOR_SPACE,
			.imageExtent = g_image_extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = LogicalDevice::g_QUEUE_FAMILY_COUNT,
			.pQueueFamilyIndices = PhysicalDevice::g_queue_family_indices.data(),
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = g_PRESENT_MODE,
			.clipped = VK_TRUE,
		};

		Vulkan::check(vkCreateSwapchainKHR(g_device, &g_status, nullptr, &swapchain), "Failed to create swapchain");

		return swapchain;
	}

	VkExtent2D get_image_extent() {
		VkSurfaceCapabilitiesKHR surface_capabilities{};
		Vulkan::check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice::g_physical_device, g_surface, &surface_capabilities), "get_image_extent: failed to get surface capabilities");
		
		VkExtent2D image_extent{ surface_capabilities.currentExtent.width, surface_capabilities.currentExtent.height };

		if(g_image_extent.width == 0xFFFFFFFF) {
			glfwGetFramebufferSize(Window::g_glfw_window, reinterpret_cast<int*>(&g_image_extent.width), reinterpret_cast<int*>(&g_image_extent.height));
		}

		return image_extent;
	}

	void check_format_colorspace(VkFormat format, VkColorSpaceKHR colorspace) {
		uint32_t format_colorspace_count{};
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice::g_physical_device, g_surface, &format_colorspace_count, nullptr);
		std::vector<VkSurfaceFormatKHR> format_colorspace_pairs(format_colorspace_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice::g_physical_device, g_surface, &format_colorspace_count, format_colorspace_pairs.data());

		bool supported = false;

		for(int i = 0; i < format_colorspace_count && !supported; ++i) {
			if (format_colorspace_pairs[i].format == format && format_colorspace_pairs[i].colorSpace == colorspace) {
				supported = true;
			}
		}

		if(!supported) {
			throw std::runtime_error("check_format_colorspace: requested format colorspace pair not supported");
		}
	}

	void check_present_mode(VkPresentModeKHR present_mode) {
		uint32_t present_mode_count{};
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice::g_physical_device, g_surface, &present_mode_count, nullptr);
		std::vector<VkPresentModeKHR> present_modes(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice::g_physical_device, g_surface, &present_mode_count, present_modes.data());

		bool supported = false;

		for(int i = 0; i < present_mode_count && !supported; ++i) {
			if(present_modes[i] == present_mode) {
				supported = true;
			}
		}

		if(!supported) {
			throw std::runtime_error("check_present_mode: requested present mode not supported");
		}
	}
}
