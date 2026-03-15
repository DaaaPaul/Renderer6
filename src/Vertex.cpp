#include "Vertex.hpp"

namespace Vertex {
	VkVertexInputBindingDescription Vertex::getInputBinding() {
		return VkVertexInputBindingDescription{
			.binding = 0,
			.stride = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::getInputAttributes() {
		return { 
			VkVertexInputAttributeDescription{
				.location = 0,
				.binding = 0,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(Vertex, position)
			}, 
			VkVertexInputAttributeDescription{
				.location = 1,
				.binding = 0,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(Vertex, color)
			}, 
			VkVertexInputAttributeDescription{
				.location = 2,
				.binding = 0,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = offsetof(Vertex, texCoord)
			}
		};
	}
}
