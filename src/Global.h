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
		inline uint32_t gFramesInFlight = 0xFFFFFFFF; /* (*) */

		void load();
		inline bool gLoaded = false;

		[[nodiscard]] ::Engine::Frames& getFrames();
		[[nodiscard]] Vertex::Transforms& getCurrentTransformation();
	}

	inline constexpr uint32_t gPARTICLES_COUNT = 8192;
	inline constexpr uint32_t gPARTICLES_BUFFER_SIZE = gPARTICLES_COUNT * sizeof(Particle::Particle);
	inline uint32_t gVertexBufferSize = 0xFFFFFFFF; /* (*) */
	inline uint32_t gIndexBufferSize = 0xFFFFFFFF; /* (*) */

	void load();
	inline bool gLoaded = false;

	std::pair<std::vector<Vertex::Vertex>, std::vector<uint32_t>>& getGltfModel();
	[[nodiscard]] ktxTexture2 const* getKtxTexture2();
	[[nodiscard]] std::vector<Particle::Particle> getParticlesData();
	[[nodiscard]] Backend::Window& getWindow();
	[[nodiscard]] Backend::Instance& getInstance();
	[[nodiscard]] Backend::Devices& getDevices();
	[[nodiscard]] Backend::Swapchain& getSwapchain();
	[[nodiscard]] std::vector<VkImage>& getSwapchainImages();
	[[nodiscard]] DeviceMemory::HostVisible& getHostVisibleMemory();
	[[nodiscard]] DeviceMemory::DeviceLocal& getDeviceLocalMemory();
	[[nodiscard]] ::Engine::PipelineLayout& getModelPipelineLayout();
	[[nodiscard]] ::Engine::PipelineLayout& getParticlePipelineLayout();
	[[nodiscard]] ::Engine::ShaderModule& getModelShaderModule();
	[[nodiscard]] ::Engine::ShaderModule& getParticleShaderModule();
	[[nodiscard]] ::Engine::GraphicsPipeline& getModelGraphicsPipeline();
	[[nodiscard]] ::Engine::GraphicsPipeline& getParticlesGraphicsPipeline();
	[[nodiscard]] ::Engine::ComputePipeline& getComputePipeline();
}