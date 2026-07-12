#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <type_traits>
#include <cstdint>

struct Vertex {
	glm::vec4 position{};
	glm::vec2 tex_coord{};

	static constexpr VkVertexInputBindingDescription get_vertex_input_binding_description(uint32_t binding_num)  {
		return {
			.binding = binding_num,
			.stride = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};
	}
	static constexpr std::vector<VkVertexInputAttributeDescription> get_vertex_input_attributes(uint32_t binding_num)  {
		return { 
			VkVertexInputAttributeDescription{
				.location = 0,
				.binding = binding_num,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(Vertex, position)
			}, 
			VkVertexInputAttributeDescription{
				.location = 1,
				.binding = binding_num,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = offsetof(Vertex, tex_coord)
			}
		};
	}
};