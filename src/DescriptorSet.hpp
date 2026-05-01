#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Resource {
	class DescriptorSet {
		private:
		VkDescriptorPool pool{};
		VkDescriptorSet set{};
		VkDescriptorSetLayout layout{};

		public:
		explicit DescriptorSet(std::vector<VkDescriptorSetLayoutBinding> const&);
		~DescriptorSet();

		private:
		static VkDescriptorSetLayout createLayout(std::vector<VkDescriptorSetLayoutBinding> const&);
		static VkDescriptorPool createPool(std::vector<VkDescriptorSetLayoutBinding> const&);
		static VkDescriptorSet createDescriptorSet(VkDescriptorSetLayout, VkDescriptorPool);
	};
}