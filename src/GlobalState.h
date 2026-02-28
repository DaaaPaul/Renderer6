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
		static [[nodiscard]] ktxTexture2 const* getKtxTexture2();
		static [[nodiscard]] Backend::Window& getWindow();
		static [[nodiscard]] Backend::Instance& getInstance();
		static [[nodiscard]] Backend::Devices& getDevices();
		static [[nodiscard]] Backend::Swapchain& getSwapchain();
		static [[nodiscard]] DeviceMemory::HostVisible& getHostVisibleMemory();
		static [[nodiscard]] DeviceMemory::DeviceLocal& getDeviceLocalMemory();
		static [[nodiscard]] Engine::GraphicsPipeline& getGraphicsPipeline();
	};
}