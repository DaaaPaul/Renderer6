#include "Particle.hpp"

namespace Particle {
	[[nodiscard]] VkDescriptorSetLayoutBinding Particle::getDescriptorSetBinding(uint32_t const& BINDING_NUMBER) {
		return VkDescriptorSetLayoutBinding{
			.binding = BINDING_NUMBER,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
		};
	}

	VkVertexInputBindingDescription getInputBinding() {
		return VkVertexInputBindingDescription{
			.binding = 0,
			.stride = sizeof(Particle),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};
	}

	std::vector<VkVertexInputAttributeDescription> getInputAttributes() {
		return { 
			VkVertexInputAttributeDescription{
				.location = 0,
				.binding = 0,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = offsetof(Particle, position)
			}, 
			VkVertexInputAttributeDescription{
				.location = 1,
				.binding = 0,
				.format = VK_FORMAT_R32G32B32_SFLOAT,
				.offset = offsetof(Particle, color)
			}, 
		};
	}

	namespace DeltaTime {
		VkDescriptorSetLayoutBinding getDescriptorSetBinding(uint32_t const& BINDING_NUMBER) {
			return VkDescriptorSetLayoutBinding{
				.binding = BINDING_NUMBER,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
			};
		}
	}
}