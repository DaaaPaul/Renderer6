#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <iostream>
#include <stdexcept>
#include "Vertex.hpp"
#include "Transforms.hpp"
#include "Window.hpp"
#include "Instance.hpp"
#include "Devices.hpp"
#include "Swapchain.hpp"
#include "DeviceMemoryCommon.h"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "GraphicsPipeline.hpp"
#include "Engine.h"

namespace GlobalState {
	class Core {
		private:
		[[nodiscard]] ktxTexture2 const* getKtxTexture2() const;
		[[nodiscard]] Backend::Window& getWindow() const;
		[[nodiscard]] Backend::Instance& getInstance() const;

	};

		Backend::Devices& fGetDevicesWrapper() {
			static Backend::Devices gDevices(
				&getInstance(),
				Backend::Devices::sGetConstructParameters(getInstance().instance)
			);

			return gDevices;
		}

		Backend::Swapchain& fGetSwapchainWrapper() {
			static Backend::Swapchain gSwapchainWrapper(
				&fGetDevicesWrapper(),
				Backend::Swapchain::sGetConstructParameters(
					getInstance().instance,
					fGetDevicesWrapper().mpPhysicalDevice,
					getInstance().WINDOW->glfwWindow,
					fGetDevicesWrapper().mGRAPHICS_QUEUE_FAMILY_INDEX
				)
			);

			return gSwapchainWrapper;
		}

		const DeviceMemory::Common::HostVisibleConstructArguements fGetHostVisibleMemoryConstructArguements();
		void fPopulateHostVisibleMemory(DeviceMemory::HostVisible& toBePopulated);
		const DeviceMemory::Common::DeviceLocalConstructArguements fGetDeviceLocalMemoryConstructArguements();
		void fPopulateDeviceLocalMemory(DeviceMemory::DeviceLocal& toBePopulated);

		DeviceMemory::HostVisible& fGetHostVisibleMemory() {
			static DeviceMemory::HostVisible gHostVisibleMemory(
				&fGetDevicesWrapper(),
				&fGetHostVisibleMemoryConstructArguements,
				&fPopulateHostVisibleMemory
			);

			return gHostVisibleMemory;
		}

		DeviceMemory::DeviceLocal& fGetDeviceLocalMemory() {
			static DeviceMemory::DeviceLocal gDeviceLocalMemory(
				&fGetDevicesWrapper(),
				&fGetDeviceLocalMemoryConstructArguements,
				&fPopulateDeviceLocalMemory
			);

			return gDeviceLocalMemory;
		}

		Backend::GraphicsPipeline& fGetGraphicsPipeline() {
			static const std::vector<VkDescriptorSetLayout> gDESCRIPTOR_SET_LAYOUTS{
				[&]() -> std::vector<VkDescriptorSetLayout> {
					std::vector<VkDescriptorSetLayout> initialValue{ fGetHostVisibleMemory().mDescriptorpSetLayouts };
					initialValue.insert(initialValue.end(), fGetDeviceLocalMemory().mDescriptorpSetLayouts.begin(), fGetDeviceLocalMemory().mDescriptorpSetLayouts.end());
					return initialValue;
				}()
			};

			static Backend::GraphicsPipeline gGraphicsPipeline(
				&fGetDevicesWrapper(),
				Backend::GraphicsPipeline::sGetConstructParameters(gDESCRIPTOR_SET_LAYOUTS)
			);

			return gGraphicsPipeline;
		}
	};
}