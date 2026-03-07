#include "Particle.hpp"
	
VkVertexInputBindingDescription Particle::getInputBindingDescription() {
	const VkVertexInputBindingDescription PARTICLE_INPUT_BINDING = {
		.binding = 1,
		.stride = sizeof(Particle),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	return PARTICLE_INPUT_BINDING;
}

std::vector<VkVertexInputAttributeDescription> Particle::getInputAttributeDescriptions() {
	const VkVertexInputAttributeDescription POSITION_ATTRIBUTE = {
		.location = 3,
		.binding = 1,
		.format = VK_FORMAT_R32G32B32A32_SFLOAT,
		.offset = offsetof(Particle, position)
	};
	const VkVertexInputAttributeDescription COLOR_ATTRIBUTE = {
		.location = 4,
		.binding = 1,
		.format = VK_FORMAT_R32G32B32A32_SFLOAT,
		.offset = offsetof(Particle, color)
	};
	const VkVertexInputAttributeDescription VELOCITY_ATTRIBUTE = {
		.location = 5,
		.binding = 1,
		.format = VK_FORMAT_R32G32_SFLOAT,
		.offset = offsetof(Particle, velocity)
	};

	return { POSITION_ATTRIBUTE, COLOR_ATTRIBUTE, VELOCITY_ATTRIBUTE };
}