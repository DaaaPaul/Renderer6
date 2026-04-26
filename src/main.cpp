#include <iostream>
#include <stdexcept>
#include "OldResources.h"
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
#include "ImageViewHotspot.h"
#include "FrameData.h"

int main() {
    try {
		Resources::loadModel();
		Backend::Window::init();
		Resources::loadParticles();
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
		ImageViewHotspot::clear();

		Engine::Pipelines::clear();
		Engine::ShaderModule::clear();
		Engine::PipelineLayouts::clear();
		Engine::Swapchain::deInit();

		Memory::Device::deInit();
		Memory::Host::deInit();

		Backend::LogicalDevice::deInit();
		Backend::Instance::deInit();
		Backend::Window::deInit();
	} catch(std::runtime_error const& RT_ERROR) {
        std::cerr << "ERROR: " << RT_ERROR.what() << "\n";
    }

    return 0;
}