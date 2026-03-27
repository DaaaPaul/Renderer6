#pragma once

#include "Vertex.hpp"
#include "Transforms.hpp"
#include "Particle.hpp"
#include "Window.hpp"
#include "Instance.hpp"
#include "Devices.h"
#include "Swapchain.hpp"
#include "Memory.h"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "PipelineLayout.h"
#include "ShaderModule.hpp"
#include "GraphicsPipeline.h"
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

	[[nodiscard]] Memory::HostVisible& getHostVisibleMemory();
	[[nodiscard]] Memory::DeviceLocal& getDeviceLocalMemory();
	[[nodiscard]] ::Engine::PipelineLayout& getModelPipelineLayout();
	[[nodiscard]] ::Engine::PipelineLayout& getEmptyPipelineLayout();
	[[nodiscard]] ::Engine::PipelineLayout& getComputePipelineLayout();
	[[nodiscard]] ::Engine::ShaderModule& getModelShaderModule();
	[[nodiscard]] ::Engine::ShaderModule& getParticleShaderModule();
	[[nodiscard]] ::Engine::GraphicsPipeline& getModelGraphicsPipeline();
	[[nodiscard]] ::Engine::GraphicsPipeline& getParticlesGraphicsPipeline();
	[[nodiscard]] ::Engine::ComputePipeline& getComputePipeline();
}