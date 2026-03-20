#pragma once

#include "Util.h"

namespace Memory {
	namespace Device {
		inline VkDeviceMemory gpMemory{};
		inline std::vector<Util::Memory::BufferBundle> gBuffers{};
		inline std::vector<Util::Memory::ImageBundle> gImages{};
		inline std::vector<VkSampler> gSamplers{};
		inline VkDescriptorPool gDescriptorPool{};
		inline std::vector<Util::Memory::DescriptorSetBundle> gDescriptorSets{};

		inline std::vector<VkBufferCreateInfo> gBufferCreates{};
		inline std::vector<VkMemoryRequirements> gBufferMemoryRequirements{};

		inline std::vector<VkImageCreateInfo> gImageCreates{};
		inline std::vector<VkMemoryRequirements> gImageMemoryRequirements{};

		inline std::vector<VkSamplerCreateInfo> gSamplerCreates{};

		inline VkDescriptorPoolCreateInfo gDescriptorPoolCreate{};
		inline std::vector<VkDescriptorSetLayoutCreateInfo> gDescriptorSetLayoutCreates{};
		inline std::vector<VkDescriptorSetAllocateInfo> gDescriptorSetAllocates{};

		void init();
		void deInit();

		void initMemoryResources();
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

		void populateSamplerCreates() noexcept;
		void createSamplers();

		void populateDescriptorPoolCreate() noexcept;
		void populateDescriptorSetLayoutCreates() noexcept;
		void populateDescriptorSetAllocates() noexcept;
		void createDescriptorSets();

		void deInitMemoryResources() noexcept;
		void deInitDescriptorResources() noexcept;

		namespace Mutate {

		}
	}
}