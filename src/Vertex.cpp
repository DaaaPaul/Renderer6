#include <vector>
#include "Vertex.hpp"

namespace Vertex {
	VkVertexInputBindingDescription Vertex::getInputBinding(uint32_t const& BINDING_NUM) {
		return VkVertexInputBindingDescription{
			.binding = BINDING_NUM,
			.stride = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::getInputAttributes(uint32_t const& BINDING_NUM) {
		return { 
			VkVertexInputAttributeDescription{
				.location = 0,
				.binding = BINDING_NUM,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(Vertex, position)
			}, 
			VkVertexInputAttributeDescription{
				.location = 1,
				.binding = BINDING_NUM,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = offsetof(Vertex, texCoord)
			}
		};
	}
}
