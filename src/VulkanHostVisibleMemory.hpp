#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanMemoryCommon.h"

struct VulkanHostVisibleMemory {
	VulkanDevicesWrapper* mpVulkanDevicesWrapper{};
	VkDeviceMemory mpHostVisibleMemory{};
	std::vector<VkBuffer> mHostVisiblepBuffers{};
	std::vector<VulkanMemoryCommon::BufferInfo> mHostVisibleBufferInfos{};
	std::vector<VkDeviceSize> mBufferOffsets{};
	std::vector<VkDeviceSize> mBufferSizes{};
	VkDescriptorPool mpDescriptorPool{};
	std::vector<VulkanMemoryCommon::DescriptorSetInfo> mDeviceLocalDescriptorSetInfos{};
	std::vector<VkDescriptorSetLayout> mDescriptorpSetLayouts{};
	std::vector<VkDescriptorSet> mDescriptorpSets{};

	void writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES);
	void createDescriptorSet(VulkanMemoryCommon::DescriptorSetInfo const& INFO, size_t const& INDEX);
	void updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);

	explicit VulkanHostVisibleMemory(VulkanDevicesWrapper* pGivenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::BufferInfo> const& GIVEN_BUFFER_INFO, std::vector<VulkanMemoryCommon::DescriptorSetInfo> const& GIVEN_DESCRIPTOR_SET_INFOS);
	~VulkanHostVisibleMemory();

	DELETE_COPY_CONSTRUCTORS(VulkanHostVisibleMemory)
	DELETE_MOVE_CONSTRUCTORS(VulkanHostVisibleMemory)
};