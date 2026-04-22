#include "Particle.hpp"

namespace Particle {
	VkVertexInputBindingDescription Particle::getInputBinding(uint32_t const& BINDING_NUM) {
		return VkVertexInputBindingDescription{
			.binding = BINDING_NUM,
			.stride = sizeof(Particle),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};
	}

	std::vector<VkVertexInputAttributeDescription> Particle::getInputAttributes(uint32_t const& BINDING_NUM) {
		return { 
			VkVertexInputAttributeDescription{
				.location = 0,
				.binding = BINDING_NUM,
				.format = VK_FORMAT_R32G32B32A32_SFLOAT,
				.offset = offsetof(Particle, color)
			}, 
			VkVertexInputAttributeDescription{
				.location = 1,
				.binding = BINDING_NUM,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = offsetof(Particle, position)
			}, 
		};
	}

	VkDescriptorSetLayoutBinding Particle::getDescriptorSetBinding(uint32_t const& BINDING_NUM) {
		return VkDescriptorSetLayoutBinding{
			.binding = BINDING_NUM,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
		};
	}

	namespace Delta {
		VkDescriptorSetLayoutBinding getDescriptorSetBinding(uint32_t const& BINDING_NUM) {
			return VkDescriptorSetLayoutBinding{
				.binding = BINDING_NUM,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
			};
		}
	}
}