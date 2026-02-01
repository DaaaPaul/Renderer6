#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanMemoryCommon.h"

struct VulkanDeviceLocalMemory {
	VulkanDevicesWrapper* mpVulkanDevicesWrapper{};
	VkDeviceMemory mpDeviceLocalMemory{};
	std::vector<VkBuffer> mDeviceLocalpBuffers{};
	std::vector<VulkanMemoryCommon::VulkanBufferInfo> mDeviceLocalBufferInfos{};
	std::vector<VkDeviceSize> mBufferOffsets{};
	std::vector<VkDeviceSize> mBufferSizes{};

	void copyToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS);

	explicit VulkanDeviceLocalMemory(VulkanDevicesWrapper* pGivenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::VulkanBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO);
	~VulkanDeviceLocalMemory();

	DELETE_COPY_CONSTRUCTORS(VulkanDeviceLocalMemory)
	DELETE_MOVE_CONSTRUCTORS(VulkanDeviceLocalMemory)
};