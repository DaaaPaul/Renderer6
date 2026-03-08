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

	[[nodiscard]] VkDescriptorSetLayoutBinding DeltaTime::getDescriptorSetBinding(uint32_t const& BINDING_NUMBER) {
		return VkDescriptorSetLayoutBinding{
			.binding = BINDING_NUMBER,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
		};
	}
}