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
#include "GraphicsPipeline.hpp"
#include "ComputePipeline.hpp"
#include "Engine.h"

namespace Global {
	namespace Engine {
		inline uint32_t gHitmanIndex = 0;
		inline uint32_t gHitmenInFlight = 0xFFFFFFFF; /* (*) */
		inline uint64_t gTimelineValue = 0;

		void load();
		inline bool gGlobalEngineLoaded = false;

		[[nodiscard]] ::Engine::Killhouse& getKillhouse();
		[[nodiscard]] Vertex::Transforms& getCurrentTransformation();
	}

	inline constexpr uint32_t gPARTICLES_COUNT = 8192;
	inline uint32_t gVertexBufferSize = 0xFFFFFFFF; /* (*) */
	inline uint32_t gIndexBufferSize = 0xFFFFFFFF; /* (*) */
	inline uint32_t gParticleBufferSize = 0xFFFFFFFF; /* (*) */

	void load();
	inline bool gGlobalLoaded = false;

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
	[[nodiscard]] ::Engine::GraphicsPipeline& getGraphicsPipeline();
	[[nodiscard]] ::Engine::ComputePipeline& getComputePipeline();
}