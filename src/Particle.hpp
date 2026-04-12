#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include "Util.h"

namespace Particle {
	struct Particle {
		glm::vec4 color{};
		glm::vec2 position{};
		glm::vec2 velocity{};

		static std::vector<VkVertexInputAttributeDescription> getInputAttributes(uint32_t const&);
		static VkVertexInputBindingDescription getInputBinding(uint32_t const&);
		static VkDescriptorSetLayoutBinding getDescriptorSetBinding(uint32_t const&);
	};

	namespace DeltaTime {
		VkDescriptorSetLayoutBinding getDescriptorSetBinding(uint32_t const&);
	}
}