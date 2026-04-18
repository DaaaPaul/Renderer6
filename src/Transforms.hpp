#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>
#include <cstdint>

namespace Vertex {
	struct Transforms {
		glm::mat4 model{};
		glm::mat4 view{};
		glm::mat4 projection{};

		Transforms() : 
			model{}, view{}, projection{} {}
		Transforms(glm::mat4 const& MODEL, glm::mat4 const& VIEW, glm::mat4 const& PROJECTION) :
			model{ MODEL }, view{ VIEW }, projection{ PROJECTION } {
			projection[1][1] *= -1.0f;	
		}

		static constexpr VkDescriptorSetLayoutBinding getDescriptorSetBinding(uint32_t const& BINDING_NUMBER);
	};
}