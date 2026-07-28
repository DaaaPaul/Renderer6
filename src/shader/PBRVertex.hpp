#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <type_traits>
#include <cstdint>

struct PBRVertex {
	glm::vec3 position{};
	glm::vec3 normal{};
	glm::vec2 uv{};
	glm::vec4 tangent{};

	static constexpr VkVertexInputBindingDescription get_vertex_input_binding_description(uint32_t binding_num)  {
		return {
			.binding = binding_num,
			.stride = sizeof(PBRVertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};
	}
	static constexpr std::vector<VkVertexInputAttributeDescription> get_vertex_input_attributes(uint32_t binding_num)  {
		return { 
			VkVertexInputAttributeDescription{
				.location = 0,
				.binding = binding_num,
				.format = VK_FORMAT_R32G32B32_SFLOAT,
				.offset = offsetof(PBRVertex, position)
			}, 
			VkVertexInputAttributeDescription{
				.location = 1,
				.binding = binding_num,
				.format = VK_FORMAT_R32G32B32_SFLOAT,
				.offset = offsetof(PBRVertex, normal)
			}, 
			VkVertexInputAttributeDescription{
				.location = 2,
				.binding = binding_num,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = offsetof(PBRVertex, uv)
			}, 
			VkVertexInputAttributeDescription{
				.location = 3,
				.binding = binding_num,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(PBRVertex, tangent)
			}, 
		};
	}
};