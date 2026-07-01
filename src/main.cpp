#include <iostream>
#include <stdexcept>
#include <chrono>
#include "Backend/Window.h"
#include "Backend/Instance.h"
#include "Backend/LogicalDevice.h"
#include "Backend/PhysicalDevice.h"
#include "Backend/Swapchain.h"
#include "Backend/PipelineLayouts.h"
#include "Backend/ShaderModule.h"
#include "Backend/Pipelines.h"
#include "Utility/Vulkan.h"
#include "Utility/Utility.h"
#include "Memory/MemoryManager.h"
#include "Engine/Engine.h"
#include "Engine/EntityManager.h"
#include "Engine/FrameKits.h"

int main() {
    try {
		std::chrono::steady_clock::time_point startup(std::chrono::high_resolution_clock::now());

		Window::init();
		Instance::init();
		Vulkan::load();
		PhysicalDevice::init();
		LogicalDevice::init();
		Swapchain::init();

		MemoryManager::init();

		PipelineLayouts::init();
		ShaderModule::init();
		Pipelines::init();

		FrameKits::init();
		EntityManager::init();

		std::chrono::steady_clock::time_point ready(std::chrono::high_resolution_clock::now());
		std::cout << "LOAD TIME: " << std::chrono::duration<float, std::chrono::seconds::period>(ready - startup).count() << '\n';
		
		Engine::run();

		EntityManager::destroy();
		FrameKits::destroy();

		Pipelines::clear();
		ShaderModule::clear();
		PipelineLayouts::clear();

		MemoryManager::destroy();

		Swapchain::destroy();
		LogicalDevice::destroy();
		Instance::destroy();
		Window::destroy();
	} catch(const std::runtime_error& runtime_error) {
        PRINTLN("ERROR: " << runtime_error.what());
    }
}