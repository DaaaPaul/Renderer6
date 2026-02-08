#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include "DeviceMemoryCommon.h"
#include "Common.h"

namespace DeviceMemory {
	namespace Common {
		unsigned char* fStbiLoadImage(const char* const& FILE_PATH, int& width, int& height, int& bytesPerPixel) {
			unsigned char* pData{ stbi_load(FILE_PATH, &width, &height, &bytesPerPixel, 4) };

			CHECK_NULLPTR(pData, "Failed to load image with stbi")

			std::cout << "Loaded " << width << "x" << height << " image from " << FILE_PATH << " with " << bytesPerPixel << " components individually\n";
			return pData;
		}

		[[nodiscard]] VkBuffer fCreateBuffer(VkDevice pLogicalDevice, BufferInfo const& INFO) {
			VkBufferCreateInfo bufferInfo{
				.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size = INFO.mBUFFER_SIZE,
				.usage = INFO.mBUFFER_USAGE,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 1,
				.pQueueFamilyIndices = &INFO.mGRAPHICS_QUEUE_FAMILY_INDEX,
			};

			VkBuffer returnBuffer{};
			vkCreateBuffer(pLogicalDevice, &bufferInfo, nullptr, &returnBuffer);

			return returnBuffer;
		}

		[[nodiscard]] std::pair<VkDeviceSize, std::vector<VkDeviceSize>> fGetMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS) {
			std::pair<VkDeviceSize, std::vector<VkDeviceSize>> allocationSizeAndBufferOffsets{};
			uint32_t buffersCount = static_cast<uint32_t>(BUFFER_MEMORY_REQUIREMENTS.size());
			allocationSizeAndBufferOffsets.second.resize(buffersCount, UINT64_MAX);

			for (int i = 0; i < buffersCount; i++) {
				while (allocationSizeAndBufferOffsets.first % BUFFER_MEMORY_REQUIREMENTS[i].alignment != 0) {
					allocationSizeAndBufferOffsets.first++;
				}

				allocationSizeAndBufferOffsets.second[i] = allocationSizeAndBufferOffsets.first;

				allocationSizeAndBufferOffsets.first += BUFFER_MEMORY_REQUIREMENTS[i].size;
			}

			return allocationSizeAndBufferOffsets;
		}

		[[nodiscard]] uint32_t fGetMemoryTypeIndex(VkPhysicalDevice pPhysicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES) {
			uint32_t finalMemoryRequirementsMask = UINT32_MAX;
			for (VkMemoryRequirements const& BUFFER_MEMORY_REQUIREMENT : BUFFER_MEMORY_REQUIREMENTS) {
				finalMemoryRequirementsMask &= BUFFER_MEMORY_REQUIREMENT.memoryTypeBits;
			}

			VkPhysicalDeviceMemoryProperties memoryProperties{};
			vkGetPhysicalDeviceMemoryProperties(pPhysicalDevice, &memoryProperties);

			uint32_t memoryTypeIndexReturn{ UINT32_MAX };
			for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
				if ((finalMemoryRequirementsMask & (1 << i)) &&
					((memoryProperties.memoryTypes[i].propertyFlags & MEMORY_PROPERTIES) == MEMORY_PROPERTIES)) {
					memoryTypeIndexReturn = i;
				}
			};

			return memoryTypeIndexReturn;
		}

		[[nodiscard]] VkDescriptorPool fCreateDescriptorPool(VkDevice pLogicalDevice, std::vector<DescriptorSetInfo> const& INFO) {
			VkDescriptorPool pReturnDescriptorPool{};
		
			// create pool sizes (ASSUMING UNIQUE DESCRIPTOR TYPE PER ITS OWN UNIQUE BINDING)
			std::vector<VkDescriptorPoolSize> poolSizes{};
			for(Common::DescriptorSetInfo const& CUSTOM_SET_INFO : INFO) {
				const std::vector<VkDescriptorSetLayoutBinding> BINDINGS{ CUSTOM_SET_INFO.mLAYOUT_BINDINGS };

				for(VkDescriptorSetLayoutBinding const& BINDING : BINDINGS) {
					poolSizes.emplace_back(BINDING.descriptorType, BINDING.descriptorCount);
				}
			}

			// create descriptor pool
			const VkDescriptorPoolCreateInfo DESCRIPTOR_POOL_INFO{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
				.maxSets = static_cast<uint32_t>(INFO.size()),
				.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
				.pPoolSizes = poolSizes.data(),
			};
			CHECK_VK_SUCCESS(
				vkCreateDescriptorPool(pLogicalDevice, &DESCRIPTOR_POOL_INFO, nullptr, &pReturnDescriptorPool),
				"Failed to create descriptor pool"
			)

			return pReturnDescriptorPool;
		}
	}
}
