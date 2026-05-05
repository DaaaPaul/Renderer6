#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Resource {
	class DescriptorSet {
		public:
		struct Write {
			VkWriteDescriptorSet info{};
			VkDescriptorImageInfo imageInfo{};
			VkDescriptorBufferInfo bufferInfo{};
			VkBufferView bufferViewInfo{};
		};

		private:
		VkDescriptorPool pool{};
		VkDescriptorSet set{};
		VkDescriptorSetLayout layout{};
		Write write{};

		public:
		explicit DescriptorSet(std::vector<VkDescriptorSetLayoutBinding> const&, Write const&);
		~DescriptorSet();

		void bind();

		private:
		static VkDescriptorSetLayout createLayout(std::vector<VkDescriptorSetLayoutBinding> const&);
		static VkDescriptorPool createPool(std::vector<VkDescriptorSetLayoutBinding> const&);
		static VkDescriptorSet createDescriptorSet(VkDescriptorSetLayout, VkDescriptorPool);
	};
}