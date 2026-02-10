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
		const uint32_t SWAPCHAIN_IMAGE_COUNT{ GlobalState::gSwapchainWrapper.mParameters.mSwapchainKHRCreateInfo.minImageCount };
		const uint32_t GRAPHICS_QF_INDEX{ GlobalState::gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX };

		std::cout << "Running with swapchain image count of " << SWAPCHAIN_IMAGE_COUNT << "\n";
		std::cout << "Running with graphics qf index of " << GRAPHICS_QF_INDEX << "\n";

		Engine::ImageKillhouse killhouse(SWAPCHAIN_IMAGE_COUNT, GRAPHICS_QF_INDEX);
		Engine::fRenderLoop(killhouse);
	} catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cout << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}