#pragma once

#include "Devices.hpp"
#include "Common.h"
#include "DeviceMemoryCommon.h"

namespace DeviceMemory {
	struct DeviceLocal {
		Backend::Devices* mpDevices{};
		VkDeviceMemory mpDeviceLocalMemory{};
		std::vector<VkBuffer> mDeviceLocalpBuffers{};
		std::vector<Common::BufferInfo> mDeviceLocalBufferInfos{};
		std::vector<VkDeviceSize> mBufferOffsets{};
		std::vector<VkDeviceSize> mBufferSizes{};

		VkDescriptorPool mpDescriptorPool{};
		std::vector<Common::DescriptorSetInfo> mDeviceLocalDescriptorSetInfos{};
		std::vector<VkDescriptorSetLayout> mDescriptorpSetLayouts{};
		std::vector<VkDescriptorSet> mDescriptorpSets{};

		void copyToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS);
		void createDescriptorSet(Common::DescriptorSetInfo const& INFO, size_t const& INDEX);
		void updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);

		DeviceLocal();
		explicit DeviceLocal(Backend::Devices* pGivenDevices, std::vector<Common::BufferInfo> const& GIVEN_BUFFER_INFO, std::vector<Common::DescriptorSetInfo> const& GIVEN_DESCRIPTOR_SET_INFOS, void (*pBootFunction)());
		~DeviceLocal();

		DELETE_COPY_CONSTRUCTORS(DeviceLocal)
		DELETE_MOVE_CONSTRUCTORS(DeviceLocal)
	};
}