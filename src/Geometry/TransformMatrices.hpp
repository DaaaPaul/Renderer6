#pragma once

#include <vulkan/vulkan_core.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cstdint>

struct TransformMatrices {
	glm::mat4 model_matrix{};
	glm::mat4 view_matrix{};
	glm::mat4 projection_matrix{};

	TransformMatrices(const glm::mat4& model_matrix, const glm::mat4& view_matrix, const glm::mat4& projection_matrix) :
		model_matrix(model_matrix), view_matrix(view_matrix), projection_matrix(projection_matrix) {
		this->projection_matrix[1][1] *= -1.0f;	
	}

	static constexpr VkDescriptorSetLayoutBinding get_descriptor_set_binding(const uint32_t& binding_num) {
		return VkDescriptorSetLayoutBinding{
			.binding = binding_num,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		};
	}
};

std::ostream& operator<<(std::ostream& os, TransformMatrices const& transform_matrices);