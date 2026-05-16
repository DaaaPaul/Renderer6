#pragma once

#include "Utility.h"

namespace Memory {
	namespace Device {
		inline VkDeviceMemory gMemory{};
		inline std::vector<Utility::Memory::BufferBundle> gBuffers{};
		inline std::vector<Utility::Memory::ImageBundle> gImages{};
		inline std::vector<VkSampler> gSamplers{};
		inline VkDescriptorPool gDescriptorPool{};
		inline std::vector<Utility::Memory::DescriptorSetBundle> gDescriptorSets{};

		inline std::vector<VkBufferCreateInfo> gBufferCreates{};
		inline std::vector<VkMemoryRequirements> gBufferMemoryRequirements{};

		inline std::vector<VkImageCreateInfo> gImageCreates{};
		inline std::vector<VkMemoryRequirements> gImageMemoryRequirements{};

		inline std::vector<VkMemoryRequirements> gAllMemoryRequirements{};
		inline std::vector<VkDeviceSize> gMemoryOffsets{};
		inline std::vector<Utility::Memory::ItemType> gMemoryItemTypes{};

		inline std::vector<VkSamplerCreateInfo> gSamplerCreates{};

		inline VkDescriptorPoolCreateInfo gDescriptorPoolCreate{};
		inline std::vector<VkDescriptorPoolSize> gDescriptorPoolSizes{};
		inline std::vector<VkDescriptorSetLayoutCreateInfo> gDescriptorSetLayoutCreates{};
		inline std::vector<std::vector<VkDescriptorSetLayoutBinding>> gDescriptorSetLayoutCreateBindings{};
		inline std::vector<VkDescriptorSetAllocateInfo> gDescriptorSetAllocates{};

		void init();
		void destroy();

		void initMemoryResources();
		void populateSamplerCreates();
		void createSamplers();
		void initDescriptorResources();

		void populateBufferCreates();
		void createBuffers();
		void populateBufferMemoryRequirements();
		void populateImageCreates();
		void createImages();
		void populateImageMemoryRequirements();
		void createMemory();
		void bind_buffers();
		void populateBufferAddresses();
		void bindImages();
		void initializeBufferData();
		void initializeImageData();

		void populateDescriptorSetLayoutCreates();
		void populateDescriptorPoolCreate();
		void createDescriptorSetLayouts();
		void createDescriptorPool();
		void populateDescriptorSetAllocates();
		void createDescriptorSets();
		void writeToDescriptorSets();

		void deInitMemoryResources();
		void destroySamplers();
		void deInitDescriptorResources();

		namespace Mutate {
			void copyToBuffer(uint32_t const&, VkBuffer, std::vector<VkBufferCopy> const&);
			void copyToImage(uint32_t const&, VkBuffer, std::vector<VkBufferImageCopy> const&);
			void bindSampledImage(uint32_t const&, uint32_t const&, uint32_t const&);
			void bindSampler(uint32_t const&, uint32_t const&, uint32_t const&);
		}
	}
}