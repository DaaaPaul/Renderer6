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
		inline static bool loaded = false;

		public:
		static void load();

		// only call these after the call to load()
		[[nodiscard]] static ktxTexture2 const* getKtxTexture2();
		[[nodiscard]] static Backend::Window& getWindow();
		[[nodiscard]] static Backend::Instance& getInstance();
		[[nodiscard]] static Backend::Devices& getDevices();
		[[nodiscard]] static Backend::Swapchain& getSwapchain();
		[[nodiscard]] static DeviceMemory::HostVisible& getHostVisibleMemory();
		[[nodiscard]] static DeviceMemory::DeviceLocal& getDeviceLocalMemory();
		[[nodiscard]] static Engine::GraphicsPipeline& getGraphicsPipeline();
	};
}