#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "Devices.hpp"

namespace Backend {
	class Swapchain {
		public:
		struct CreateInfo {
			VkSurfaceKHR surface{};
			VkSwapchainCreateInfoKHR createInfo{};
			std::vector<uint32_t> accessorQfIndices{};

			void reroutePointers() {
				createInfo.surface = surface;
			}
			CreateInfo(VkSurfaceKHR&& salvageSurface, VkSwapchainCreateInfoKHR const& GIVEN_CREATE_INFO, std::vector<uint32_t> const& GIVEN_ACCESSOR_QFs) :
				surface{ salvageSurface },
				createInfo{ GIVEN_CREATE_INFO }, 
				accessorQfIndices{ GIVEN_ACCESSOR_QFs } {
				reroutePointers();
			}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				surface{ salvageCreateInfo.surface },
				createInfo{ salvageCreateInfo.createInfo }, 
				accessorQfIndices(std::move(salvageCreateInfo.accessorQfIndices)) {
				reroutePointers();
			}
			DELETE_COPY_CONSTRUCTORS(CreateInfo)
		};

		private:
		Devices* devices{};
		VkSwapchainKHR swapchain{};
		VkSurfaceKHR surface{};
		const CreateInfo CREATE_INFO;

		public:
		void recreateThyself();

		private:
		static void sCheckHaveVkFormatColorspace(Swapchain const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE);
		static void sCheckHavePresentModeKHR(Swapchain const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE);

		public:
		explicit Swapchain(Devices* givenDevices, CreateInfo&& salvageCreateInfo);
		~Swapchain();
		[[nodiscard]] Devices*& getDevices() { return devices; }
		[[nodiscard]] VkSwapchainKHR& getSwapchain() { return swapchain; }
		[[nodiscard]] CreateInfo const& getCreateInfo() const { return CREATE_INFO; }

		DELETE_COPY_CONSTRUCTORS(Swapchain)
		DELETE_MOVE_CONSTRUCTORS(Swapchain)
	};
}
