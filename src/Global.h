#pragma once

#include "Vertex.hpp"
#include "Transforms.hpp"
#include "Particle.hpp"
#include "Window.hpp"
#include "Instance.hpp"
#include "Devices.hpp"
#include "Swapchain.hpp"
#include "DeviceMemory.h"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "PipelineLayout.hpp"
#include "ShaderModule.hpp"
#include "GraphicsPipeline.hpp"
#include "ComputePipeline.hpp"
#include "Frames.hpp"

namespace Global {
	namespace Engine {
		inline uint32_t gFrameIndex = 0;
		inline uint32_t gFramesInFlight = UINT32_MAX; /* (*) */

		void asserts() noexcept;
		void load();
		inline bool gLoaded = false;

		[[nodiscard]] ::Engine::Frames& getFrames();
		[[nodiscard]] Vertex::Transforms& getCurrentTransformation();
	}

	void asserts() noexcept;
	void load();
	inline bool gLoaded = false;

	[[nodiscard]] Backend::Swapchain& getSwapchain();
	[[nodiscard]] DeviceMemory::HostVisible& getHostVisibleMemory();
	[[nodiscard]] DeviceMemory::DeviceLocal& getDeviceLocalMemory();
	[[nodiscard]] ::Engine::PipelineLayout& getModelPipelineLayout();
	[[nodiscard]] ::Engine::PipelineLayout& getEmptyPipelineLayout();
	[[nodiscard]] ::Engine::PipelineLayout& getComputePipelineLayout();
	[[nodiscard]] ::Engine::ShaderModule& getModelShaderModule();
	[[nodiscard]] ::Engine::ShaderModule& getParticleShaderModule();
	[[nodiscard]] ::Engine::GraphicsPipeline& getModelGraphicsPipeline();
	[[nodiscard]] ::Engine::GraphicsPipeline& getParticlesGraphicsPipeline();
	[[nodiscard]] ::Engine::ComputePipeline& getComputePipeline();
}