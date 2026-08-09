#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <iostream>
#include <stdexcept>
#include <chrono>
#include "backend/Window.h"
#include "backend/Instance.h"
#include "backend/LogicalDevice.h"
#include "backend/PhysicalDevice.h"
#include "backend/Swapchain.h"
#include "pipeline/PipelineLayouts.h"
#include "pipeline/ShaderModule.h"
#include "pipeline/Pipelines.h"
#include "utility/Vulkan.h"
#include "utility/Utility.h"
#include "memory/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/EntityManager.h"
#include "engine/Frame.hpp"
#include "gui/Gui.h"

int main() {
    try {
		std::chrono::steady_clock::time_point startup(std::chrono::high_resolution_clock::now());

		Window::init();
		Instance::init();
		Vulkan::load_functions();
		PhysicalDevice::init();
		LogicalDevice::init();
		Vulkan::init_vma();
		Swapchain::init();

		MemoryManager::init();

		PipelineLayouts::init();
		ShaderModule::init();
		Pipelines::init();

		EntityManager::init();
		Gui::init(Window::g_glfw_window, Swapchain::g_image_extent.width, Swapchain::g_image_extent.height);

		std::chrono::steady_clock::time_point ready(std::chrono::high_resolution_clock::now());
		Utility::println(std::to_string(std::chrono::duration<float, std::chrono::seconds::period>(ready - startup).count()));
		
		Engine::run();

		Gui::destroy();
		EntityManager::destroy();

		Pipelines::clear();
		ShaderModule::clear();
		PipelineLayouts::clear();

		MemoryManager::destroy();

		Swapchain::destroy();
		Vulkan::destroy_vma();
		LogicalDevice::destroy();
		Instance::destroy();
		Window::destroy();
	} catch(const std::runtime_error& runtime_error) {\
		Utility::println("------------------------------ERROR------------------------------");
        Utility::println(runtime_error.what());
		Utility::println("-----------------------------------------------------------------");
    }
}