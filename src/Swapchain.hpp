#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "Devices.hpp"

namespace Backend {
	class Swapchain {
		public:
		struct CreateInfo {
			VkSurfaceKHR pSurface{};
			VkSwapchainCreateInfoKHR createInfo{};
			std::vector<uint32_t> accessorQfIndices{};

			void reroutePointers() {
				createInfo.surface = pSurface;
			}
			CreateInfo(VkSurfaceKHR&& salvageSurface, VkSwapchainCreateInfoKHR const& GIVEN_CREATE_INFO, std::vector<uint32_t>&& salvageAccessorQfIndices) :
				pSurface{ salvageSurface },
				createInfo{ GIVEN_CREATE_INFO }, 
				accessorQfIndices{ std::move(salvageAccessorQfIndices) } {
				reroutePointers();
			}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				pSurface{ salvageCreateInfo.pSurface },
				createInfo{ salvageCreateInfo.createInfo }, 
				accessorQfIndices(std::move(salvageCreateInfo.accessorQfIndices)) {
				reroutePointers();
			}
		};

		private:
		Devices* pDevices{};
		VkSwapchainKHR pSwapchain{};
		VkSurfaceKHR pSurface{};
		const CreateInfo CREATE_INFO;

		public:
		void recreate();
		[[nodiscard]] VkExtent2D getCurrentExtent() const noexcept;

		private:
		static void checkHaveFormatColorspace(Swapchain const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE);
		static void checkHavePresentModeKHR(Swapchain const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE);

		public:
		explicit Swapchain(Devices* pGivenDevices, CreateInfo&& salvageCreateInfo);
		~Swapchain();
		[[nodiscard]] Devices*& getDevices() { return pDevices; }
		[[nodiscard]] VkSwapchainKHR& getSwapchain() { return pSwapchain; }
		[[nodiscard]] CreateInfo const& getCreateInfo() const { return CREATE_INFO; }

		DELETE_COPY_CONSTRUCTORS(Swapchain)
		DELETE_MOVE_CONSTRUCTORS(Swapchain)
	};
}
