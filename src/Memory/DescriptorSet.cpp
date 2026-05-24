#include "DescriptorSet.hpp"
#include "Backend/LogicalDevice.h"
#include "Utility/Utility.h"

DescriptorSet::DescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings) :
	pool{ Vulkan::create_one_set_pool(bindings) }, 
	layout{ Vulkan::create_descriptor_set_layout(bindings) }, 
	descriptor_set{ Vulkan::create_descriptor_set(layout, pool) } {

}

void DescriptorSet::destroy() noexcept {
	vkFreeDescriptorSets(g_device, pool, 1, &descriptor_set);
	vkDestroyDescriptorSetLayout(g_device, layout, nullptr);
	vkDestroyDescriptorPool(g_device, pool, nullptr);
}

void DescriptorSet::write(Write write_info) {
	VkDescriptorImageInfo descriptor_image_info{
		.sampler = write_info.sampler,
		.imageView = write_info.image_view,
		.imageLayout = write_info.image_layout
	};

	VkWriteDescriptorSet write_descriptor_set{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = descriptor_set,
		.dstBinding = write_info.binding_num,
		.dstArrayElement = write_info.descriptor_num,
		.descriptorCount = write_info.descriptor_count,
		.descriptorType = write_info.descriptor_type,
		.pImageInfo = &descriptor_image_info
	};

	vkUpdateDescriptorSets(g_device, 1, &write_descriptor_set, 0, nullptr);
}
