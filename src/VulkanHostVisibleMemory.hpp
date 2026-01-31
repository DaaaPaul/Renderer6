#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "Common.h"
#include "VulkanPFNs.h"

struct VulkanHostVisibleMemoryBufferInfo {
	const VkDeviceSize BUFFER_SIZE{};
	const VkBufferUsageFlags BUFFER_USAGE{};
	const uint32_t GRAPHICS_QUEUE_FAMILY_INDEX{};
};

struct VulkanHostVisibleMemory {
	VulkanDevicesWrapper* mVulkanDevicesWrapper{};
	VkDeviceMemory mHostVisibleMemory{};
	std::vector<VkBuffer> mHostVisibleBuffers{};
	std::vector<VulkanHostVisibleMemoryBufferInfo> mHostVisibleBufferInfos{};

	VulkanHostVisibleMemory(VulkanDevicesWrapper* givenVulkanDevicesWrapper, std::vector<VulkanHostVisibleMemoryBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO);
	~VulkanHostVisibleMemory();
	[[nodiscard]] static VkBuffer createBuffer(VulkanHostVisibleMemoryBufferInfo const& info);

	DELETE_COPY_CONSTRUCTORS(VulkanHostVisibleMemory)
	DELETE_MOVE_CONSTRUCTORS(VulkanHostVisibleMemory)
};