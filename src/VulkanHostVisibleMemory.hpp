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
	VkDescriptorPool mpDescriptorPool{};
	std::vector<VulkanMemoryCommon::VulkanDescriptorSetInfo> mDeviceLocalDescriptorSetInfos{};
	std::vector<VkDescriptorSetLayout> mDescriptorpSetLayouts{};
	std::vector<VkDescriptorSet> mDescriptorpSets{};

	void writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES);
	void createDescriptorSet(VulkanMemoryCommon::VulkanDescriptorSetInfo const& INFO, size_t const& INDEX);
	void updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);

	explicit VulkanHostVisibleMemory(VulkanDevicesWrapper* pGivenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::VulkanBufferInfo> const& GIVEN_BUFFER_INFO, std::vector<VulkanMemoryCommon::VulkanDescriptorSetInfo> const& GIVEN_DESCRIPTOR_SET_INFOS);
	~VulkanHostVisibleMemory();

	DELETE_COPY_CONSTRUCTORS(VulkanHostVisibleMemory)
	DELETE_MOVE_CONSTRUCTORS(VulkanHostVisibleMemory)
};