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
#include "DeviceMemory.h"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "GraphicsPipeline.hpp"
#include "Engine.h"

namespace GlobalState {
	namespace Core {
		void load();
		std::pair<std::vector<Vertex::Vertex>, std::vector<uint32_t>>& getGltfModel();
		[[nodiscard]] ktxTexture2 const* getKtxTexture2();
		[[nodiscard]] Backend::Window& getWindow();
		[[nodiscard]] Backend::Instance& getInstance();
		[[nodiscard]] Backend::Devices& getDevices();
		[[nodiscard]] Backend::Swapchain& getSwapchain();
		[[nodiscard]] DeviceMemory::HostVisible& getHostVisibleMemory();
		[[nodiscard]] DeviceMemory::DeviceLocal& getDeviceLocalMemory();
		[[nodiscard]] Engine::GraphicsPipeline& getGraphicsPipeline();
	};
}