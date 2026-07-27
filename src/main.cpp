#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <iostream>
#include <stdexcept>
#include <chrono>
#include "Backend/Window.h"
#include "Backend/Instance.h"
#include "Backend/LogicalDevice.h"
#include "Backend/PhysicalDevice.h"
#include "Backend/Swapchain.h"
#include "Pipeline/PipelineLayouts.h"
#include "Pipeline/ShaderModule.h"
#include "Pipeline/Pipelines.h"
#include "Utility/Vulkan.h"
#include "Utility/Utility.h"
#include "Memory/MemoryManager.h"
#include "Engine/Engine.h"
#include "Engine/EntityManager.h"
#include "Engine/Frame.hpp"

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

		EntityManager::init();

		std::chrono::steady_clock::time_point ready(std::chrono::high_resolution_clock::now());
		Utility::println(std::to_string(std::chrono::duration<float, std::chrono::seconds::period>(ready - startup).count()));
		
		Engine::run();

		EntityManager::destroy();

		Pipelines::clear();
		ShaderModule::clear();
		PipelineLayouts::clear();

		MemoryManager::destroy();

		Swapchain::destroy();
		LogicalDevice::destroy();
		Instance::destroy();
		Window::destroy();
	} catch(const std::runtime_error& runtime_error) {\
		Utility::println("------------------------------ERROR------------------------------");
        Utility::println(runtime_error.what());
		Utility::println("-----------------------------------------------------------------");
    }
}