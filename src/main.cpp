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
		GlobalState::load();

		Engine::ImageKillhouse killhouse(Engine::gHitmenInFlight, GlobalState::getDevices().getGraphicsQfIndex());
		Engine::renderLoop(killhouse);
	} catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cerr << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}