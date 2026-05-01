#include "DescriptorSet.hpp"
#include "LogicalDevice.h"
#include "Util.h"

namespace Resource {
	explicit DescriptorSet::DescriptorSet(std::vector<VkDescriptorSetLayoutBinding> const& BINDINGS) {
		layout = createLayout(BINDINGS);
		pool = createPool(BINDINGS);
		set = createDescriptorSet(layout, pool);
	}

	DescriptorSet::~DescriptorSet() {
		vkFreeDescriptorSets(gDevice, pool, 1, &set);
		vkDestroyDescriptorSetLayout(gDevice, layout, nullptr);
		vkDestroyDescriptorPool(gDevice, pool, nullptr);
	}

	VkDescriptorSetLayout DescriptorSet::createLayout(std::vector<VkDescriptorSetLayoutBinding> const& BINDINGS) {
		VkDescriptorSetLayout layout{};

		VkDescriptorSetLayoutCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = UINT32(BINDINGS.size()),
			.pBindings = BINDINGS.data()
		};

		CHECK_VK_SUCCESS(vkCreateDescriptorSetLayout(gDevice, &create, nullptr, &layout), "createLayout: failed")

		return layout;
	}

	VkDescriptorPool DescriptorSet::createPool(std::vector<VkDescriptorSetLayoutBinding> const& BINDINGS) {
		VkDescriptorPool pool{};
		std::vector<VkDescriptorPoolSize> poolSizes{};

		for(VkDescriptorSetLayoutBinding const& B : BINDINGS) {
			poolSizes.push_back(VkDescriptorPoolSize{ .type = B.descriptorType, .descriptorCount = B.descriptorCount });
		}
		VkDescriptorPoolCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = 1,
			.poolSizeCount = UINT32(poolSizes.size()),
			.pPoolSizes = poolSizes.data()
		};
		
		CHECK_VK_SUCCESS(vkCreateDescriptorPool(gDevice, &create, nullptr, &pool), "createPool: failed")

		return pool;
	}

	VkDescriptorSet DescriptorSet::createDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorPool pool) {
		VkDescriptorSet set{};

		VkDescriptorSetAllocateInfo create{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = pool,
			.descriptorSetCount = 1,
			.pSetLayouts = &layout,
		};

		CHECK_VK_SUCCESS(vkAllocateDescriptorSets(gDevice, nullptr, &set), "createDescriptorSet: failed")

		return set;
	}
}