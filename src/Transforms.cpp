#include "Transforms.hpp"

namespace Vertex {
	[[nodiscard]] const VkDescriptorSetLayoutBinding Transforms::sGetTransformationMatricesDescriptorSetLayoutBinding(uint32_t const& BINDING_NUMBER) {
		const VkDescriptorSetLayoutBinding RETURN_LAYOUT_BINDING{
			.binding = BINDING_NUMBER,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		};

		return RETURN_LAYOUT_BINDING;
	}
}