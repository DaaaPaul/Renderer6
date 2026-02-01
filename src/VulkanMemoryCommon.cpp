#include "VulkanMemoryCommon.h"

namespace VulkanMemoryCommon {
	[[nodiscard]] VkBuffer fCreateBuffer(VkDevice logicalDevice, VulkanBufferInfo const& INFO) {
		VkBufferCreateInfo bufferInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = INFO.mBUFFER_SIZE,
			.usage = INFO.mBUFFER_USAGE,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &INFO.mGRAPHICS_QUEUE_FAMILY_INDEX,
		};

		VkBuffer returnBuffer{};
		VulkanPFNs::gpVkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &returnBuffer);

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

	[[nodiscard]] uint32_t fGetMemoryTypeIndex(VkPhysicalDevice physicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES) {
		uint32_t finalMemoryRequirementsMask = UINT32_MAX;
		for (VkMemoryRequirements const& BUFFER_MEMORY_REQUIREMENT : BUFFER_MEMORY_REQUIREMENTS) {
			finalMemoryRequirementsMask &= BUFFER_MEMORY_REQUIREMENT.memoryTypeBits;
		}

		VkPhysicalDeviceMemoryProperties memoryProperties{};
		VulkanPFNs::gpVkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

		uint32_t memoryTypeIndexReturn{ UINT32_MAX };
		for (int i = 0; i < memoryProperties.memoryTypeCount; i++) {
			if ((finalMemoryRequirementsMask & (1 << i)) &&
				((memoryProperties.memoryTypes[i].propertyFlags & MEMORY_PROPERTIES) == MEMORY_PROPERTIES)) {
				memoryTypeIndexReturn = i;
			}
		};

		return memoryTypeIndexReturn;
	}
}