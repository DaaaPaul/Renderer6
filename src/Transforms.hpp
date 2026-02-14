#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include "Common.h"

namespace Vertex {
	struct Transforms {
		glm::mat4 mModel{};
		glm::mat4 mView{};
		glm::mat4 mProjection{};

		[[nodiscard]] const static VkDescriptorSetLayoutBinding sGetTransformationMatricesDescriptorSetLayoutBinding(uint32_t const& BINDING_NUMBER);
	};
}