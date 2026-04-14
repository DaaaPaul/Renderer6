#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include "Transforms.hpp"

namespace Engine {
	inline Vertex::Transforms gTransformation{};

	void recordComputeCommands(VkCommandBuffer& pCommandBuffer);
	void recordDrawModelCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX);
	void recordDrawParticlesCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX);
	void renderNext();
	void run();

	void initTransformation() noexcept;
	void setViewportAndScissor(VkCommandBuffer& cmdBuffer);
	void resize();
	float getDeltaTime() noexcept;
	void updateTransformation();
	void updateDeltaTime();
}
