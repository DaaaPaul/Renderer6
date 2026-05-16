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
	} catch(const std::runtime_error& runtime_error) {
        PRINTLN("ERROR: " << runtime_error.what());
    }
}