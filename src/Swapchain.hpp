#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "Devices.hpp"

namespace Backend {
	struct Swapchain {
		struct SwapchainConstructInfo {
			VkSurfaceKHR mpSurfaceKHR{};
			VkSwapchainCreateInfoKHR mSwapchainKHRCreateInfo{};
			const uint32_t mGRAPHICS_QUEUE_FAMILY_INDEX{};
		};

		Devices* devices{};
		VkSwapchainKHR mpSwapchainKHR{};
		VkSurfaceKHR mpSurfaceKHR{};
		SwapchainConstructInfo mParameters{};

		void recreateThyself();

		[[nodiscard]] static SwapchainConstructInfo sGetConstructParameters(VkInstance pInstance, VkPhysicalDevice pPhysicalDevice, GLFWwindow* pGlfwWindow, uint32_t const& GRAPHICS_QUEUE_FAMILY_INDEX);
		static void sCheckHaveVkFormatColorspace(Swapchain const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE);
		static void sCheckHavePresentModeKHR(Swapchain const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE);

		Swapchain();
		explicit Swapchain(Devices* givenDevices, SwapchainConstructInfo const& GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO);
		~Swapchain();

		DELETE_COPY_CONSTRUCTORS(Swapchain)
		DELETE_MOVE_CONSTRUCTORS(Swapchain)
	};
}
