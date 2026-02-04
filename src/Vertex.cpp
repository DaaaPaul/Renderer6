#include "Vertex.hpp"

const VkVertexInputBindingDescription Vertex::getInputBindingDescription() {
	const VkVertexInputBindingDescription VERTEX_INPUT_BINDING = {
		.binding = 0,
		.stride = sizeof(Vertex),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	return VERTEX_INPUT_BINDING;
}

const std::vector<VkVertexInputAttributeDescription> Vertex::getInputAttributeDescriptions() {
	const VkVertexInputAttributeDescription POSITION_ATTRIBUTE = {
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32A32_SFLOAT,
		.offset = offsetof(Vertex, mPOSITION)
	};
	const VkVertexInputAttributeDescription COLOR_ATTRIBUTE = {
		.location = 1,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32A32_SFLOAT,
		.offset = offsetof(Vertex, mCOLOR)
	};

	return { POSITION_ATTRIBUTE, COLOR_ATTRIBUTE };
}