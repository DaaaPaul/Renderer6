#include <iostream>
#include <stdexcept>
#include "Backend/Window.h"
#include "Backend/Instance.h"
#include "Backend/LogicalDevice.h"
#include "Backend/PhysicalDevice.h"
#include "Backend/Swapchain.h"
#include "Backend/PipelineLayouts.h"
#include "Backend/ShaderModule.h"
#include "Backend/Pipelines.h"
#include "Engine/Engine.h"
#include "Engine/FrameKits.h"

int main() {
    try {
		Resources::load_model();
		Window::init();
		Instance::init();
		PhysicalDevice::init();
		LogicalDevice::init();
		Swapchain::init();

		Memory::Host::init();
		Memory::Device::init();

		PipelineLayouts::init();
		ShaderModule::init();
		Pipelines::init();

		FrameKits::init();
		
		Engine::run();

		FrameKits::destroy();

		Pipelines::clear();
		ShaderModule::clear();
		PipelineLayouts::clear();
		Swapchain::destroy();

		Memory::Device::destroy();
		Memory::Host::destroy();

		LogicalDevice::destroy();
		Instance::destroy();
		Window::destroy();
	} catch(const std::runtime_error& runtime_error) {
        PRINTLN("ERROR: " << runtime_error.what());
    }
}