#include "Vertex.hpp"

namespace Vertex {
	VkVertexInputBindingDescription Vertex::getInputBinding() {
		const VkVertexInputBindingDescription VERTEX_INPUT_BINDING = {
			.binding = 0,
			.stride = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};

		return VERTEX_INPUT_BINDING;
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::getInputAttributes() {
		const VkVertexInputAttributeDescription POSITION_ATTRIBUTE = {
			.location = 0,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset = offsetof(Vertex, position)
		};
		const VkVertexInputAttributeDescription COLOR_ATTRIBUTE = {
			.location = 1,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset = offsetof(Vertex, color)
		};
		const VkVertexInputAttributeDescription TEXTURE_COORDINATE_ATTRIBUTE = {
			.location = 2,
			.binding = 0,
			.format = VK_FORMAT_R32G32_SFLOAT,
			.offset = offsetof(Vertex, texCoord)
		};

		return { POSITION_ATTRIBUTE, COLOR_ATTRIBUTE, TEXTURE_COORDINATE_ATTRIBUTE };
	}
}
