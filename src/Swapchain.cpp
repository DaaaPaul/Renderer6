#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Swapchain.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Instance.h"
#include "Window.h"

namespace Swapchain {
	void init() {
		createSurface();

		populateImageSize();
		checkImageFormatAndColorspaceSupported();
		checkPresentModeSupported();

		populateCurrentSwapchainStatus();
		createSwapchain();
		populateImages();
	}

	void destroy() {
		destroySwapchain();
		destroySurface();
	}

	void recreate() {
		populateImageSize();
		while(gImageSize.width == 0 && gImageSize.height == 0) {
			glfwWaitEvents(); // wait for the next glfw event, and process it when it comes
			populateImageSize(); // query window size after glfw event is processed
		}

		destroySwapchain();
		populateCurrentSwapchainStatus();
		createSwapchain();
		populateImages();
	}

	void createSurface() {
		VK_CHECK(glfwCreateWindowSurface(Instance::g_instance, Window::g_glfw_window, nullptr, &gSurface), "Failed to create surface")
	}

	void populateCurrentSwapchainStatus() {
		gStatus = VkSwapchainCreateInfoKHR{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = gSurface,
			.minImageCount = g_IMAGE_COUNT,
			.imageFormat = gIMAGE_FORMAT,
			.imageColorSpace = gIMAGE_COLOR_SPACE,
			.imageExtent = gImageSize,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = LogicalDevice::g_QUEUE_FAMILY_COUNT,
			.pQueueFamilyIndices = PhysicalDevice::g_queue_family_indices.data(),
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = gPRESENT_MODE,
			.clipped = VK_TRUE,
		};
	}

	void createSwapchain() {
		VK_CHECK(vkCreateSwapchainKHR(g_device, &gStatus, nullptr, &g_swapchain), "Failed to create swapchain")
	}

	void populateImages() {
		uint32_t imageCount = UINT32_MAX;
		vkGetSwapchainImagesKHR(g_device, g_swapchain, &imageCount, nullptr);
		gImages.clear();
		gImages.resize(imageCount, {});
		vkGetSwapchainImagesKHR(g_device, g_swapchain, &imageCount, gImages.data());
	}

	void populateImageSize() {
		VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice::g_physical_device, gSurface, &surfaceCapabilities), "Failed to get surface capabilities")
		
		gImageSize = VkExtent2D(surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);

		if(gImageSize.width == UINT32_MAX) {
			glfwGetFramebufferSize(Window::g_glfw_window, reinterpret_cast<int*>(&gImageSize.width), reinterpret_cast<int*>(&gImageSize.height));
		}
	}

	void checkImageFormatAndColorspaceSupported() {
		uint32_t supportedFormatColorspacePairCount{};
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice::g_physical_device, gSurface, &supportedFormatColorspacePairCount, nullptr);
		std::vector<VkSurfaceFormatKHR> supportedFormatColorspacePairs(supportedFormatColorspacePairCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice::g_physical_device, gSurface, &supportedFormatColorspacePairCount, supportedFormatColorspacePairs.data());

		bool supported = false;

		for(int i = 0; i < supportedFormatColorspacePairCount && !supported; ++i) {
			if (supportedFormatColorspacePairs[i].format == gIMAGE_FORMAT && supportedFormatColorspacePairs[i].colorSpace == gIMAGE_COLOR_SPACE) {
				supported = true;
			}
		}

		if(!supported) {
			throw std::runtime_error("Swapchain format and colorspace pair not supported");
		}
	}

	void checkPresentModeSupported() {
		uint32_t supportedPresentModeCount{};
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice::g_physical_device, gSurface, &supportedPresentModeCount, nullptr);
		std::vector<VkPresentModeKHR> supportedPresentModes(supportedPresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice::g_physical_device, gSurface, &supportedPresentModeCount, supportedPresentModes.data());

		bool supported = false;

		for(int i = 0; i < supportedPresentModeCount && !supported; ++i) {
			if(supportedPresentModes[i] == gPRESENT_MODE) {
				supported = true;
			}
		}

		if(!supported) {
			throw std::runtime_error("Swapchain present mode not supported");
		}
	}

	void destroySurface() {
		vkDestroySurfaceKHR(Instance::g_instance, gSurface, nullptr);
	}		

	void destroySwapchain() {
		vkDestroySwapchainKHR(g_device, g_swapchain, nullptr);
	}
}
