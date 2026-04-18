#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include "Transforms.hpp"

namespace Engine {
	inline Vertex::Transforms gTransformation{};
	void initTransformation() noexcept;

	void recordComputeCommands(VkCommandBuffer cmdBuffer);
	void recordDrawModelCommands(VkCommandBuffer cmdBuffer, uint32_t const& IMAGE_INDEX);
	void recordDrawParticlesCommands(VkCommandBuffer cmdBuffer, uint32_t const& IMAGE_INDEX);
	void renderNext();
	void run();

	bool acquireSwapchainImage(uint32_t& INDEX, VkFence fenceToSignal);
	bool presentSwapchainImage(uint32_t const& INDEX, VkQueue queue);
	void waitForTimelineSemaphore(VkSemaphore timeline, uint64_t const& WAIT_VAL) noexcept;
	void waitForFence(VkFence fence) noexcept;
	void resetAndBeginCmdBuffer(VkCommandBuffer cmdBuffer) noexcept;
	void setViewportAndScissor(VkCommandBuffer cmdBuffer);
	void resize();
	float getDeltaTime() noexcept;
	void updateTransformation();
	void updateDeltaTime();
}
