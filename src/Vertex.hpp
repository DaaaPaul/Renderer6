#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <type_traits>
#include <cstdint>

struct Vertex {
	glm::vec4 position{};
	glm::vec2 tex_coord{};

	static constexpr VkVertexInputBindingDescription get_vertex_input_binding_description(const uint32_t& binding_num)  {
		return {
			.binding = binding_num,
			.stride = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};
	}
	static constexpr std::vector<VkVertexInputAttributeDescription> get_vertex_input_attributes(const uint32_t& binding_num)  {
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

inline bool operator==(const Vertex& vertex_1, const Vertex& vertex_2) {
	return vertex_1.position == vertex_2.position && vertex_1.tex_coord == vertex_2.tex_coord;
}

template<> 
struct std::hash<Vertex> {
    std::size_t operator()(const Vertex& vertex) const {
        std::size_t hash_1 = std::hash<float>{}(vertex.position[2]);
        std::size_t hash_2 = std::hash<float>{}(vertex.tex_coord[1]);

        return hash_1 ^ (hash_2 << 1);
    }
};