#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <corecrt_math_defines.h>
#include <chrono>
#include "Camera.hpp"

namespace Engine {
	inline Camera gCamera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), EulerAngles{});

	void recordComputeCommands(VkCommandBuffer cmdBuffer);
	void recordDrawModelCommands(VkCommandBuffer cmdBuffer, uint32_t const& IMAGE_INDEX);
	void recordDrawParticlesCommands(VkCommandBuffer cmdBuffer, uint32_t const& IMAGE_INDEX);
	void renderNext();
	void run();

	bool acquireSwapchainImage(uint32_t& INDEX, VkFence fenceToSignal);
	bool presentSwapchainImage(uint32_t const& INDEX, VkQueue queue);
	void waitForTimelineSemaphore(VkSemaphore timeline, uint64_t const& WAIT_VAL) noexcept;
	void waitForFence(VkFence fence) noexcept;
	void beginCmdBuffer(VkCommandBuffer cmdBuffer) noexcept;
	void setViewportScissor(VkCommandBuffer cmdBuffer);
	void resize();
	void update();
	float writeDeltaTime(std::chrono::steady_clock::time_point const&, std::chrono::steady_clock::time_point const&);
}
