#include "DescriptorSet.hpp"
#include "Backend/LogicalDevice.h"
#include "Utility/Utility.h"

DescriptorSet::DescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings, const std::vector<Write>& writes) :
	writes(writes) {
	layout = create_layout(bindings);
	pool = create_pool(bindings);
	descriptor_set = create_descriptor_set(layout, pool);
}

void DescriptorSet::destroy() noexcept {
	vkFreeDescriptorSets(g_device, pool, 1, &descriptor_set);
	vkDestroyDescriptorSetLayout(g_device, layout, nullptr);
	vkDestroyDescriptorPool(g_device, pool, nullptr);
}

void DescriptorSet::write() {
	std::vector<VkWriteDescriptorSet> descriptor_writes(writes.size());
	std::vector<VkDescriptorImageInfo> descriptor_image_info(writes.size());

	for(int i = 0; i < writes.size(); i++) {
		descriptor_image_info[i] = VkDescriptorImageInfo{
			.sampler = writes[i].sampler,
			.imageView = writes[i].image_view,
			.imageLayout = writes[i].image_layout
		};

		descriptor_writes[i] = VkWriteDescriptorSet{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptor_set,
			.dstBinding = writes[i].binding_num,
			.dstArrayElement = writes[i].descriptor_num,
			.descriptorCount = writes[i].descriptor_count,
			.descriptorType = writes[i].descriptor_type,
			.pImageInfo = &descriptor_image_info[i]
		};
	}

	vkUpdateDescriptorSets(g_device, UINT32(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
}

VkDescriptorSetLayout DescriptorSet::create_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
	VkDescriptorSetLayout layout{};

	VkDescriptorSetLayoutCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = UINT32(bindings.size()),
		.pBindings = bindings.data()
	};

	VK_CHECK(vkCreateDescriptorSetLayout(g_device, &create, nullptr, &layout), "create_layout: failed")

	return layout;
}

VkDescriptorPool DescriptorSet::create_pool(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
	VkDescriptorPool pool{};
	std::vector<VkDescriptorPoolSize> poolSizes{};

	for(const VkDescriptorSetLayoutBinding& binding : bindings) {
		poolSizes.push_back(VkDescriptorPoolSize{ .type = binding.descriptorType, .descriptorCount = binding.descriptorCount });
	}
	VkDescriptorPoolCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets = 1,
		.poolSizeCount = UINT32(poolSizes.size()),
		.pPoolSizes = poolSizes.data()
	};
		
	VK_CHECK(vkCreateDescriptorPool(g_device, &create, nullptr, &pool), "create_pool: failed")

	return pool;
}

VkDescriptorSet DescriptorSet::create_descriptor_set(VkDescriptorSetLayout layout, VkDescriptorPool pool) {
	VkDescriptorSet set{};

	VkDescriptorSetAllocateInfo create{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &layout,
	};

	VK_CHECK(vkAllocateDescriptorSets(g_device, nullptr, &set), "create_descriptor_set: failed")

	return set;
}
