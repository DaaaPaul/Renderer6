#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <vector>
#include "backend/LogicalDevice.h"

class DescriptorSet {
	public:
	struct Write {
		uint32_t binding_num{};
		uint32_t descriptor_num{};
		uint32_t descriptor_count{};
		VkDescriptorType descriptor_type{};
		VkDescriptorBufferInfo buffer_info{};
		VkDescriptorImageInfo image_info{};
	};

	private:
	VkDescriptorPool pool{};
	VkDescriptorSetLayout layout{};
	VkDescriptorSet descriptor_set{};

	public:
	DescriptorSet() = default;

	explicit DescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	void destroy();

	VkDescriptorSetLayout get_layout() const {
		return layout;
	}

	VkDescriptorSet get_descriptor_set() const {
		return descriptor_set;
	}

	void write(Write write_info);
};
