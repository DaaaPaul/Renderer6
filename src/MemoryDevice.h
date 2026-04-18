#pragma once

#include "Util.h"

namespace Memory {
	namespace Device {
		inline VkDeviceMemory gMemory{};
		inline std::vector<Util::Memory::BufferBundle> gBuffers{};
		inline std::vector<Util::Memory::ImageBundle> gImages{};
		inline std::vector<VkSampler> gSamplers{};
		inline VkDescriptorPool gDescriptorPool{};
		inline std::vector<Util::Memory::DescriptorSetBundle> gDescriptorSets{};

		inline std::vector<VkBufferCreateInfo> gBufferCreates{};
		inline std::vector<VkMemoryRequirements> gBufferMemoryRequirements{};

		inline std::vector<VkImageCreateInfo> gImageCreates{};
		inline std::vector<VkMemoryRequirements> gImageMemoryRequirements{};

		inline std::vector<VkMemoryRequirements> gAllMemoryRequirements{};
		inline std::vector<VkDeviceSize> gMemoryOffsets{};
		inline std::vector<Util::Memory::ItemType> gMemoryItemTypes{};

		inline std::vector<VkSamplerCreateInfo> gSamplerCreates{};

		inline VkDescriptorPoolCreateInfo gDescriptorPoolCreate{};
		inline std::vector<VkDescriptorPoolSize> gDescriptorPoolSizes{};
		inline std::vector<VkDescriptorSetLayoutCreateInfo> gDescriptorSetLayoutCreates{};
		inline std::vector<std::vector<VkDescriptorSetLayoutBinding>> gDescriptorSetLayoutCreateBindings{};
		inline std::vector<VkDescriptorSetAllocateInfo> gDescriptorSetAllocates{};

		void init();
		void deInit();

		void initMemoryResources();
		void populateSamplerCreates() noexcept;
		void createSamplers();
		void initDescriptorResources();

		void populateBufferCreates() noexcept;
		void createBuffers();
		void populateBufferMemoryRequirements() noexcept;
		void populateImageCreates() noexcept;
		void createImages();
		void populateImageMemoryRequirements() noexcept;
		void createMemory();
		void bindBuffers();
		void populateBufferAddresses() noexcept;
		void bindImages();
		void initializeBufferData() noexcept;
		void initializeImageData() noexcept;

		void populateDescriptorSetLayoutCreates() noexcept;
		void populateDescriptorPoolCreate() noexcept;
		void createDescriptorSetLayouts();
		void createDescriptorPool();
		void populateDescriptorSetAllocates() noexcept;
		void createDescriptorSets();
		void writeToDescriptorSets();

		void deInitMemoryResources() noexcept;
		void destroySamplers() noexcept;
		void deInitDescriptorResources() noexcept;

		namespace Mutate {
			void copyToBuffer(uint32_t const&, VkBuffer, std::vector<VkBufferCopy> const&);
			void copyToImage(uint32_t const&, VkBuffer, std::vector<VkBufferImageCopy> const&);
			void bindSampledImage(uint32_t const&, uint32_t const&, uint32_t const&);
			void bindSampler(uint32_t const&, uint32_t const&, uint32_t const&);
		}
	}
}