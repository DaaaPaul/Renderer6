#include <iostream>
#include <stdexcept>
#include "Resources.h"
#include "Window.h"
#include "Instance.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include "MemoryDevice.h"
#include "MemoryHost.h"
#include "PipelineLayouts.h"
#include "ShaderModule.h"
#include "Pipelines.h"
#include "Engine.h"
#include "FrameData.h"

int main() {
    try {
		Resources::load();

		Backend::Window::init();
		Backend::Instance::init();
		Backend::PhysicalDevice::init();
		Backend::LogicalDevice::init();
		Engine::Swapchain::init();

		Memory::Host::init();
		Memory::Device::init();

		Engine::PipelineLayouts::add();
		Engine::ShaderModule::add();
		Engine::Pipelines::add();

		Engine::FrameData::init();
		
		Engine::run();

		Engine::FrameData::deInit();

		Engine::Pipelines::clear();
		Engine::ShaderModule::clear();
		Engine::PipelineLayouts::clear();
		Engine::Swapchain::deInit();

		Memory::Device::deInit();
		Memory::Host::deInit();

		Backend::LogicalDevice::deInit();
		Backend::Instance::deInit();
		Backend::Window::deInit();
	} catch(std::exception const& EXCEPTION) {
        std::cerr << "ERROR: " << EXCEPTION.what() << "\n";
    }

    return 0;
}