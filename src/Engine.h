#pragma once

#include <vulkan/vulkan.h>

namespace Engine {
	[[nodiscard]] float getDeltaTime() noexcept;
	void windowResizeRecreate();
	void freshenTransformation();
	void freshenDeltaTime();
	void recordComputeCommands(VkCommandBuffer& pCommandBuffer);
	void recordDrawModelCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX);
	void recordDrawParticlesCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX);
	void renderNext();
	void run();
}
