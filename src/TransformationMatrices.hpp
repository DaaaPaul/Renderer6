#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include "Common.h"

struct TransformationMatrices {
	glm::mat4 mModel{};
	glm::mat4 mView{};
	glm::mat4 mProjection{};

	[[nodiscard]] const static VkDescriptorSetLayoutBinding getTransformationMatricesDescriptorSetLayoutBinding(uint32_t const& BINDING_NUMBER);
};