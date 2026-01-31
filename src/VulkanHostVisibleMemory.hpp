#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "Common.h"
#include "VulkanPFNs.h"

struct VulkanBufferInfo {
	const VkDeviceSize mBUFFER_SIZE{};
	const VkBufferUsageFlags mBUFFER_USAGE{};
	const uint32_t mGRAPHICS_QUEUE_FAMILY_INDEX{};
};

struct VulkanHostVisibleMemory {
	VulkanDevicesWrapper* mVulkanDevicesWrapper{};
	VkDeviceMemory mHostVisibleMemory{};
	std::vector<VkBuffer> mHostVisibleBuffers{};
	std::vector<VulkanBufferInfo> mHostVisibleBufferInfos{};

	VulkanHostVisibleMemory(VulkanDevicesWrapper* givenVulkanDevicesWrapper, std::vector<VulkanBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO);
	~VulkanHostVisibleMemory();
	[[nodiscard]] static VkBuffer createBuffer(VulkanHostVisibleMemory const& HOST_VISIBLE_MEMORY, VulkanBufferInfo const& INFO);
	[[nodiscard]] static std::pair<VkDeviceSize, std::vector<VkDeviceSize>> getMemoryAllocationSizeAndOffsets(std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS);
	[[nodiscard]] static uint32_t getMemoryTypeIndex(VkPhysicalDevice physicalDevice, std::vector<VkMemoryRequirements> const& BUFFER_MEMORY_REQUIREMENTS, VkMemoryPropertyFlags const& MEMORY_PROPERTIES);

	DELETE_COPY_CONSTRUCTORS(VulkanHostVisibleMemory)
	DELETE_MOVE_CONSTRUCTORS(VulkanHostVisibleMemory)
};