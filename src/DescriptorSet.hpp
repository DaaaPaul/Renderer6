#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class DescriptorSet {
	public:
	struct Write {
		VkWriteDescriptorSet write_info{};
		VkDescriptorImageInfo image_info{};
		VkDescriptorBufferInfo buffer_info{};
	};

	private:
	VkDescriptorPool pool{};
	VkDescriptorSet set{};
	VkDescriptorSetLayout layout{};
	Write write{};

	public:
	explicit DescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings, const Write& write);
	~DescriptorSet();

	void bind();

	private:
	static VkDescriptorSetLayout create_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	static VkDescriptorPool create_pool(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	static VkDescriptorSet create_descriptor_set(VkDescriptorSetLayout layout, VkDescriptorPool pool);
};
