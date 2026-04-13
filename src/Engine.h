#pragma once

#include <vulkan/vulkan.h>
#include "Transforms.hpp"
#include "Swapchain.h"

namespace Engine {
	inline Vertex::Transforms gTransformation(
		glm::mat4(1.0f),
		glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::perspective(glm::radians(45.0f), static_cast<float>(Swapchain::gImageSize.width) / static_cast<float>(Swapchain::gImageSize.height), 0.1f, 100.0f)
	);

	void recordComputeCommands(VkCommandBuffer& pCommandBuffer);
	void recordDrawModelCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX);
	void recordDrawParticlesCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX);
	void renderNext();
	void run();

	void setViewportAndScissor(VkCommandBuffer& cmdBuffer);
	void resize();
	float getDeltaTime() noexcept;
	void updateTransformation();
	void updateDeltaTime();
}
