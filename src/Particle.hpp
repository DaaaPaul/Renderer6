#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include "Common.h"

namespace Particle {
	struct Particle {
		glm::vec2 position{};
		glm::vec3 color{};
		glm::vec2 velocity{};

		[[nodiscard]] static VkDescriptorSetLayoutBinding getDescriptorSetBinding(uint32_t const& BINDING_NUMBER);
	};

	struct DeltaTime {
		float deltaTime{};

		[[nodiscard]] static VkDescriptorSetLayoutBinding getDescriptorSetBinding(uint32_t const& BINDING_NUMBER);
	};

	[[nodiscard]] std::vector<VkVertexInputAttributeDescription> getInputAttributes();
	[[nodiscard]] VkVertexInputBindingDescription getInputBinding();
}