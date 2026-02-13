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
	// memory construct functions and data
	ktxTexture2 const*const gpKTX_TEXTURE{ DeviceMemory::Common::fKtxLoadImage(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\textures\Lumberjack Sion Compressed.ktx2)") };
	const DeviceMemory::Common::HostVisibleConstructArguements fGetHostVisibleMemoryConstructArguements();
	void fPopulateHostVisibleMemory(DeviceMemory::HostVisible& toBePopulated);
	const DeviceMemory::Common::DeviceLocalConstructArguements fGetDeviceLocalMemoryConstructArguements();
	void fPopulateDeviceLocalMemory(DeviceMemory::DeviceLocal& toBePopulated);

	// vulkan/rendering backend global objects
	inline Backend::Window gWindowWrapper(Backend::Window::sGetConstructParameters());
	inline Backend::Backend gBackendWrapper(&gWindowWrapper, Backend::Backend::sGetConstructParameters());
	inline Backend::Devices gDevicesWrapper(&gBackendWrapper, Backend::Devices::sGetConstructParameters(gBackendWrapper.mpInstance));
	inline Backend::Swapchain gSwapchainWrapper(&gDevicesWrapper, Backend::Swapchain::sGetConstructParameters(gBackendWrapper.mpInstance, gDevicesWrapper.mpPhysicalDevice, gBackendWrapper.mpWindow->mpGlfwWindow, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX));
	inline DeviceMemory::HostVisible gHostVisibleMemory(
		&gDevicesWrapper,
		&fGetHostVisibleMemoryConstructArguements,
		&fPopulateHostVisibleMemory
	);
	inline DeviceMemory::DeviceLocal gDeviceLocalMemory(
		&gDevicesWrapper,
		&fGetDeviceLocalMemoryConstructArguements,
		&fPopulateDeviceLocalMemory
	);
	inline Backend::GraphicsPipeline gGraphicsPipeline(
		&gDevicesWrapper,
		Backend::GraphicsPipeline::sGetConstructParameters(gHostVisibleMemory.mDescriptorpSetLayouts)
	);
}