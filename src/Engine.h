#pragma once

#include <vulkan/vulkan.h>

namespace Engine {
	[[nodiscard]] float getDeltaTime() noexcept;
	void windowResizeRecreate();
	void freshenTransformation();
	void recordDrawCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX);
	void recordComputeCommands(VkCommandBuffer& pCommandBuffer);
	void renderNext();
	void run();
}
