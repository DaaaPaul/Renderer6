#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <utility>
#include "VulkanPFNs.h"

namespace VulkanMemoryCommon {
	struct VulkanBufferInfo {
		const VkDeviceSize mBUFFER_SIZE{};
		const VkBufferUsageFlags mBUFFER_USAGE{};
		const uint32_t mGRAPHICS_QUEUE_FAMILY_INDEX{};
	};

	[[nodiscard]] VkBuffer fCreateBuffer(VkDevice logicalDevice, VulkanBufferInfo const& INFO);
	[[nodiscard]] std::pair<VkDeviceSize, std::vector<VkDeviceSize>> fGetMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS);
	[[nodiscard]] uint32_t fGetMemoryTypeIndex(VkPhysicalDevice physicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES);
}