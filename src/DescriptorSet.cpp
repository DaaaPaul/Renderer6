#include "DescriptorSet.hpp"
#include "LogicalDevice.h"
#include "Utility.h"

DescriptorSet::DescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings, const Write& write) :
	write(write) {
	layout = create_layout(bindings);
	pool = create_pool(bindings);
	set = create_descriptor_set(layout, pool);
}

DescriptorSet::~DescriptorSet() {
	vkFreeDescriptorSets(g_device, pool, 1, &set);
	vkDestroyDescriptorSetLayout(g_device, layout, nullptr);
	vkDestroyDescriptorPool(g_device, pool, nullptr);
}

void DescriptorSet::bind() {
	vkUpdateDescriptorSets(g_device, 1, &write.write_info, 0, nullptr);
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
