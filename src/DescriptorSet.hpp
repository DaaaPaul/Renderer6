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
	VkDescriptorSet descriptor_set{};
	VkDescriptorSetLayout layout{};
	std::vector<Write> writes{};

	public:
	explicit DescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings, const std::vector<Write>& writes);
	void destroy() noexcept;

	void write();

	private:
	static VkDescriptorSetLayout create_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	static VkDescriptorPool create_pool(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	static VkDescriptorSet create_descriptor_set(VkDescriptorSetLayout layout, VkDescriptorPool pool);
};
