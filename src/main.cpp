#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <iostream>
#include <stdexcept>
#include "Vertex.hpp"
#include "Window.hpp"
#include "Instance.hpp"
#include "Devices.hpp"
#include "Swapchain.hpp"
#include "DeviceMemory.h"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "GraphicsPipeline.hpp"
#include "Engine.h"
#include "GlobalState.h"

int main() {
    try {
		GlobalState::Core::load();

		const uint32_t SWAPCHAIN_IMAGE_COUNT{ GlobalState::Core::getSwapchain().getCreateInfo().createInfo.minImageCount };
		const uint32_t GRAPHICS_QF_INDEX{ GlobalState::Core::getDevices().getGraphicsQfIndex() };

		Engine::ImageKillhouse killhouse(SWAPCHAIN_IMAGE_COUNT, GRAPHICS_QF_INDEX);
		Engine::renderLoop(killhouse);
	} catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cerr << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}