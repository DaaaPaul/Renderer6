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
	void fLoadHostVisibleMemory();
	void fLoadDeviceLocalMemory();

	inline Backend::Window gWindowWrapper(Backend::Window::sGetConstructParameters());
	inline Backend::Backend gBackendWrapper(&gWindowWrapper, Backend::Backend::sGetConstructParameters());
	inline Backend::Devices gDevicesWrapper(&gBackendWrapper, Backend::Devices::sGetConstructParameters(gBackendWrapper.mpInstance));
	inline Backend::Swapchain gSwapchainWrapper(&gDevicesWrapper, Backend::Swapchain::sGetConstructParameters(gBackendWrapper.mpInstance, gDevicesWrapper.mpPhysicalDevice, gBackendWrapper.mpWindow->mpGlfwWindow, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX));
		
	inline DeviceMemory::HostVisible gHostVisibleMemory(
		&gDevicesWrapper,
		std::vector<DeviceMemory::Common::BufferInfo>{
			DeviceMemory::Common::BufferInfo(32 * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX),
			DeviceMemory::Common::BufferInfo(4 * 6, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX),
			DeviceMemory::Common::BufferInfo(sizeof(Vertex::Transforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX),
		},
		std::vector<DeviceMemory::Common::DescriptorSetInfo>{
			DeviceMemory::Common::DescriptorSetInfo({Vertex::Transforms::sGetTransformationMatricesDescriptorSetLayoutBinding(0)})
		},
		&fLoadHostVisibleMemory
	);
	inline DeviceMemory::DeviceLocal gDeviceLocalMemory(
		&gDevicesWrapper,
		std::vector<DeviceMemory::Common::BufferInfo>{
			DeviceMemory::Common::BufferInfo(32 * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX),
			DeviceMemory::Common::BufferInfo(4 * 6, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX),
		},
		{},
		&fLoadDeviceLocalMemory
	);
	inline Backend::GraphicsPipeline gGraphicsPipeline(
		&gDevicesWrapper,
		Backend::GraphicsPipeline::sGetConstructParameters(gHostVisibleMemory.mDescriptorpSetLayouts)
	);
}