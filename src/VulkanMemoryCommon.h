#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <utility>
#include "VulkanPFNs.h"

namespace VulkanMemoryCommon {
	struct BufferInfo {
		const VkDeviceSize mBUFFER_SIZE{};
		const VkBufferUsageFlags mBUFFER_USAGE{};
		const uint32_t mGRAPHICS_QUEUE_FAMILY_INDEX{};
	};

	struct DescriptorSetInfo {
		const uint32_t mBINDINGS_COUNT;
		VkDescriptorSetLayoutBinding const* mpLAYOUT_BINDINGS{};
	};

	[[nodiscard]] VkBuffer fCreateBuffer(VkDevice pLogicalDevice, BufferInfo const& INFO);
	[[nodiscard]] std::pair<VkDeviceSize, std::vector<VkDeviceSize>> fGetMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS);
	[[nodiscard]] uint32_t fGetMemoryTypeIndex(VkPhysicalDevice pPhysicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES);
	[[nodiscard]] VkDescriptorPool fCreateDescriptorPool(VkDevice pLogicalDevice, std::vector<DescriptorSetInfo> const& INFO);
}