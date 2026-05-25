#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class DescriptorSet {
	public:
	struct Write {
		uint32_t binding_num{};
		uint32_t descriptor_num{};
		uint32_t descriptor_count{};
		VkDescriptorType descriptor_type{};
		VkSampler sampler{};
		VkImageView image_view{};
		VkImageLayout image_layout{};
	};

	private:
	VkDescriptorPool pool{};
	VkDescriptorSetLayout layout{};
	VkDescriptorSet descriptor_set{};

	public:
	DescriptorSet() = default;

	explicit DescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	void destroy() noexcept;

	VkDescriptorSetLayout get_layout() const {
		return layout;
	}

	VkDescriptorSet get_descriptor_set() const {
		return descriptor_set;
	}

	void write(Write write_info);
};
