#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanMemoryCommon.h"

struct VulkanDeviceLocalMemory {
	VulkanDevicesWrapper* mVulkanDevicesWrapper{};
	VkDeviceMemory mDeviceLocalMemory{};
	std::vector<VkBuffer> mDeviceLocalBuffers{};
	std::vector<VulkanMemoryCommon::VulkanBufferInfo> mDeviceLocalBufferInfos{};
	std::vector<VkDeviceSize> mBufferOffsets{};
	std::vector<VkDeviceSize> mBufferSizes{};

	void copyToBuffer(size_t const& INDEX);

	VulkanDeviceLocalMemory(VulkanDevicesWrapper* givenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::VulkanBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO);
	~VulkanDeviceLocalMemory();

	DELETE_COPY_CONSTRUCTORS(VulkanDeviceLocalMemory)
	DELETE_MOVE_CONSTRUCTORS(VulkanDeviceLocalMemory)
};