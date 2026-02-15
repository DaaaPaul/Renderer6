#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <iostream>
#include <stdexcept>
#include "Vertex.hpp"
#include "Transforms.hpp"
#include "Window.hpp"
#include "Backend.hpp"
#include "Devices.hpp"
#include "Swapchain.hpp"
#include "DeviceMemoryCommon.h"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "GraphicsPipeline.hpp"
#include "Engine.h"

namespace GlobalState {
	inline ktxTexture2 const* fGetKtxTexture2() {
		static ktxTexture2 const*const gpKTX_TEXTURE2 = 
			DeviceMemory::Common::fKtxLoadImage(
				R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\textures\Lumberjack Sion Compressed.ktx2)"
			);

		return gpKTX_TEXTURE2;
	}

	inline Backend::Window& fGetWindowWrapper() {
		static Backend::Window gWindowWrapper(Backend::Window::sGetConstructParameters());

		return gWindowWrapper;
	}

	inline Backend::Backend& fGetBackendWrapper() {
		static Backend::Backend gBackend(
			&fGetWindowWrapper(),
			Backend::Backend::sGetConstructParameters()
		);

		return gBackend;
	}

	inline Backend::Devices& fGetDevicesWrapper() {
		static Backend::Devices gDevices(
			&fGetBackendWrapper(),
			Backend::Devices::sGetConstructParameters(fGetBackendWrapper().mpInstance)
		);

		return gDevices;
	}

	inline Backend::Swapchain& fGetSwapchainWrapper() {
		static Backend::Swapchain gSwapchainWrapper(
			&fGetDevicesWrapper(),
			Backend::Swapchain::sGetConstructParameters(
				fGetBackendWrapper().mpInstance,
				fGetDevicesWrapper().mpPhysicalDevice,
				fGetBackendWrapper().mpWindow->mpGlfwWindow,
				fGetDevicesWrapper().mGRAPHICS_QUEUE_FAMILY_INDEX
			)
		);

		return gSwapchainWrapper;
	}

	const DeviceMemory::Common::HostVisibleConstructArguements fGetHostVisibleMemoryConstructArguements();
	void fPopulateHostVisibleMemory(DeviceMemory::HostVisible& toBePopulated);
	const DeviceMemory::Common::DeviceLocalConstructArguements fGetDeviceLocalMemoryConstructArguements();
	void fPopulateDeviceLocalMemory(DeviceMemory::DeviceLocal& toBePopulated);

	inline DeviceMemory::HostVisible& fGetHostVisibleMemory() {
		static DeviceMemory::HostVisible gHostVisibleMemory(
			&fGetDevicesWrapper(),
			&fGetHostVisibleMemoryConstructArguements,
			&fPopulateHostVisibleMemory
		);

		return gHostVisibleMemory;
	}

	inline DeviceMemory::DeviceLocal& fGetDeviceLocalMemory() {
		static DeviceMemory::DeviceLocal gDeviceLocalMemory(
			&fGetDevicesWrapper(),
			&fGetDeviceLocalMemoryConstructArguements,
			&fPopulateDeviceLocalMemory
		);

		return gDeviceLocalMemory;
	}

	inline Backend::GraphicsPipeline& fGetGraphicsPipeline() {
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
}