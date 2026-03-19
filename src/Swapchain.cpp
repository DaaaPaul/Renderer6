#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Swapchain.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Instance.h"
#include "Window.h"

namespace Backend {
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

		void deInit() {
			destroySurface();
			destroySwapchain();
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
			CHECK_VK_SUCCESS(glfwCreateWindowSurface(Instance::gpInstance, Window::gpGlfwWindow, nullptr, &gpSurface), "Failed to create surface")
		}

		void populateCurrentSwapchainStatus() noexcept {
			gCurrentSwapchainStatus = VkSwapchainCreateInfoKHR{
				.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
				.surface = gpSurface,
				.minImageCount = gIMAGE_COUNT,
				.imageFormat = gIMAGE_FORMAT,
				.imageColorSpace = gIMAGE_COLOR_SPACE,
				.imageExtent = gImageSize,
				.imageArrayLayers = 1,
				.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = LogicalDevice::gQUEUE_FAMILY_COUNT,
				.pQueueFamilyIndices = PhysicalDevice::gQueueFamilyIndices.data(),
				.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
				.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
				.presentMode = gPRESENT_MODE,
				.clipped = VK_TRUE,
			};
		}

		void createSwapchain() {
			CHECK_VK_SUCCESS(vkCreateSwapchainKHR(LogicalDevice::gpDevice, &gCurrentSwapchainStatus, nullptr, &gpSwapchain), "Failed to create swapchain")
		}

		void populateImages() noexcept {
			uint32_t imageCount = UINT32_MAX;
			vkGetSwapchainImagesKHR(LogicalDevice::gpDevice, gpSwapchain, &imageCount, nullptr);
			gImages.clear();
			gImages.resize(imageCount, {});
			vkGetSwapchainImagesKHR(LogicalDevice::gpDevice, gpSwapchain, &imageCount, gImages.data());
		}

		void populateImageSize() noexcept {
			VkSurfaceCapabilitiesKHR surfaceCapabilities{};
			CHECK_VK_SUCCESS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice::gpPhysicalDevice, gpSurface, &surfaceCapabilities), "Failed to get surface capabilities")
		
			gImageSize = VkExtent2D(surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);

			if(gImageSize.width == UINT32_MAX) {
				glfwGetFramebufferSize(Window::gpGlfwWindow, reinterpret_cast<int*>(&gImageSize.width), reinterpret_cast<int*>(&gImageSize.height));
			}
		}

		void checkImageFormatAndColorspaceSupported() {
			uint32_t supportedFormatColorspacePairCount{};
			vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice::gpPhysicalDevice, gpSurface, &supportedFormatColorspacePairCount, nullptr);
			std::vector<VkSurfaceFormatKHR> supportedFormatColorspacePairs(supportedFormatColorspacePairCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice::gpPhysicalDevice, gpSurface, &supportedFormatColorspacePairCount, supportedFormatColorspacePairs.data());

			bool supported = false;

			for(int i = 0; i < supportedFormatColorspacePairCount && !supported; i++) {
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
			vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice::gpPhysicalDevice, gpSurface, &supportedPresentModeCount, nullptr);
			std::vector<VkPresentModeKHR> supportedPresentModes(supportedPresentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice::gpPhysicalDevice, gpSurface, &supportedPresentModeCount, supportedPresentModes.data());

			bool supported = false;

			for(int i = 0; i < supportedPresentModeCount && !supported; i++) {
				if(supportedPresentModes[i] == gPRESENT_MODE) {
					supported = true;
				}
			}

			if(!supported) {
				throw std::runtime_error("Swapchain present mode not supported");
			}
		}

		void destroySurface() noexcept {
			vkDestroySurfaceKHR(Instance::gpInstance, gpSurface, nullptr);
		}		

		void destroySwapchain() noexcept {
			vkDestroySwapchainKHR(LogicalDevice::gpDevice, gpSwapchain, nullptr);
		}
	}
}