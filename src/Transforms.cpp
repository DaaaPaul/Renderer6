#include "Transforms.hpp"

namespace Vertex {
	[[nodiscard]] VkDescriptorSetLayoutBinding Transforms::getDescriptorSetBinding(uint32_t const& BINDING_NUMBER) {
		return VkDescriptorSetLayoutBinding{
			.binding = BINDING_NUMBER,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		};;
	}
}