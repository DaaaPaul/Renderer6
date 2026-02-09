#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <iostream>
#include <stdexcept>
#include "Vertex.hpp"
#include "Window.hpp"
#include "Backend.hpp"
#include "Devices.hpp"
#include "Swapchain.hpp"
#include "DeviceMemoryCommon.h"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "GraphicsPipeline.hpp"
#include "Engine.h"
#include "GlobalState.h"

int main() {
    try {
		Engine::ImageKillhouse killhouse(GlobalState::gSwapchainWrapper.mParameters.mSwapchainKHRCreateInfo.minImageCount, GlobalState::gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
		Engine::fRenderLoop(killhouse);
	} catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cout << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}