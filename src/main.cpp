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
#include "FrameKits.h"

int main() {
    try {
		Resources::loadModel();
		Window::init();
		Resources::loadParticles();
		Instance::init();
		PhysicalDevice::init();
		LogicalDevice::init();
		Swapchain::init();

		Memory::Host::init();
		Memory::Device::init();

		PipelineLayouts::add();
		ShaderModule::add();
		Pipelines::add();

		FrameKits::init();
		
		Engine::run();

		FrameKits::destroy();
		ImageViewHotspot::clear();

		Pipelines::clear();
		ShaderModule::clear();
		PipelineLayouts::clear();
		Swapchain::destroy();

		Memory::Device::destroy();
		Memory::Host::destroy();

		LogicalDevice::destroy();
		Instance::destroy();
		Window::destroy();
	} catch(std::runtime_error const& RT_ERROR) {
        std::cerr << "ERROR: " << RT_ERROR.what() << "\n";
    }

    return 0;
}