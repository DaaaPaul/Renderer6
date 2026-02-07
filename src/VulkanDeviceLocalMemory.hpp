#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanMemoryCommon.h"

struct VulkanDeviceLocalMemory {
	VulkanDevicesWrapper* mpVulkanDevicesWrapper{};
	VkDeviceMemory mpDeviceLocalMemory{};
	std::vector<VulkanMemoryCommon::VulkanBufferInfo> mDeviceLocalBufferInfos{};
	std::vector<VkBuffer> mDeviceLocalpBuffers{};
	std::vector<VkDeviceSize> mBufferOffsets{};
	std::vector<VkDeviceSize> mBufferSizes{};
	VkDescriptorPool mpDescriptorPool{};
	std::vector<VulkanMemoryCommon::VulkanDescriptorSetInfo> mDeviceLocalDescriptorSetInfos{};
	std::vector<VkDescriptorSetLayout> mDescriptorpSetLayouts{};
	std::vector<VkDescriptorSet> mDescriptorpSets{};

	void copyToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS);
	void createDescriptorSet(VulkanMemoryCommon::VulkanDescriptorSetInfo const& INFO, size_t const& INDEX);
	void updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);

	explicit VulkanDeviceLocalMemory(VulkanDevicesWrapper* pGivenVulkanDevicesWrapper, std::vector<VulkanMemoryCommon::VulkanBufferInfo> const& GIVEN__BUFFER_INFO, std::vector<VulkanMemoryCommon::VulkanDescriptorSetInfo> const& GIVEN_DESCRIPTOR_SET_INFOS);
	~VulkanDeviceLocalMemory();

	DELETE_COPY_CONSTRUCTORS(VulkanDeviceLocalMemory)
	DELETE_MOVE_CONSTRUCTORS(VulkanDeviceLocalMemory)
};