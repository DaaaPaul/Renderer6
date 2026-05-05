#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <chrono>
#include "Camera.hpp"

namespace Engine {
	void imageBarrier(VkCommandBuffer cmdBuffer, VkImage image, VkImageSubresourceRange const& SUBRESOURCE_RANGE, VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX);
	
	void recordComputeCommands(VkCommandBuffer cmdBuffer);
	void recordDrawModelCommands(VkCommandBuffer cmdBuffer, uint32_t const& IMAGE_INDEX);
	void recordDrawParticlesCommands(VkCommandBuffer cmdBuffer, uint32_t const& IMAGE_INDEX);
	void renderNext();
	void run();

	bool acquireSwapchainImage(uint32_t& INDEX, VkFence fenceToSignal);
	bool presentSwapchainImage(uint32_t const& INDEX, VkQueue queue);
	void waitForTimelineSemaphore(VkSemaphore timeline, uint64_t const& WAIT_VAL);
	void waitForFence(VkFence fence);
	void beginCmdBuffer(VkCommandBuffer cmdBuffer);
	void setViewportScissor(VkCommandBuffer cmdBuffer);
	void resize();
	void update(float const&);
	float getDelta(std::chrono::steady_clock::time_point const&, std::chrono::steady_clock::time_point const&);
}
