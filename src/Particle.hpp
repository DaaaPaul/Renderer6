#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include "Common.h"

struct Particle {
	glm::vec4 position{};
	glm::vec4 color{};
	glm::vec2 velocity{};

	[[nodiscard]] static VkVertexInputBindingDescription getInputBindingDescription();
	[[nodiscard]] static std::vector<VkVertexInputAttributeDescription> getInputAttributeDescriptions();
};