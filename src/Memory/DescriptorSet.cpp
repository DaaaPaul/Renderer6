#include <vulkan/vulkan_core.h>
#include <vector>
#include "DescriptorSet.hpp"
#include "backend/LogicalDevice.h"
#include "utility/Vulkan.h"

DescriptorSet::DescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings) :
	pool{ Vulkan::create_descriptor_pool({ bindings }) }, 
	layout{ Vulkan::create_descriptor_set_layout(bindings) }, 
	descriptor_set{ Vulkan::create_descriptor_set(layout, pool) } {

}

void DescriptorSet::destroy() {
	vkFreeDescriptorSets(g_device, pool, 1, &descriptor_set);
	vkDestroyDescriptorSetLayout(g_device, layout, nullptr);
	vkDestroyDescriptorPool(g_device, pool, nullptr);
}

void DescriptorSet::write(Write write_info) {
	VkWriteDescriptorSet write_descriptor_set{
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = descriptor_set,
		.dstBinding = write_info.binding_num,
		.dstArrayElement = write_info.descriptor_num,
		.descriptorCount = write_info.descriptor_count,
		.descriptorType = write_info.descriptor_type,
	};

	if(write_info.buffer_info.buffer == VK_NULL_HANDLE) {
		write_descriptor_set.pImageInfo = &write_info.image_info;
	} else {
		write_descriptor_set.pBufferInfo = &write_info.buffer_info;
	}

	vkUpdateDescriptorSets(g_device, 1, &write_descriptor_set, 0, nullptr);
}
