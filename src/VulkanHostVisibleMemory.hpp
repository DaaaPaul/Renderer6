#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanMemoryCommon.h"

struct VulkanHostVisibleMemory {
	VulkanDevicesWrapper* mpVulkanDevicesWrapper{};
	VkDeviceMemory mpHostVisibleMemory{};
	std::vector<VkBuffer> mHostVisiblepBuffers{};
	std::vector<VulkanMemoryCommon::VulkanBufferInfo> mHostVisibleBufferInfos{};
	std::vector<VkDeviceSize> mBufferOffsets{};
	std::vector<VkDeviceSize> mBufferSizes{};

	void writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES);

	explicit VulkanHostVisibleMemory(VulkanDevicesWrapper* pGivenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::VulkanBufferInfo> const& GIVEN_VULKAN_HOST_VISIBLE_MEMORY_BUFFER_INFO);
	~VulkanHostVisibleMemory();

	DELETE_COPY_CONSTRUCTORS(VulkanHostVisibleMemory)
	DELETE_MOVE_CONSTRUCTORS(VulkanHostVisibleMemory)
};