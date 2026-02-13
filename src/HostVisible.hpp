#pragma once

#include "Devices.hpp"
#include "Common.h"
#include "DeviceMemoryCommon.h"

namespace DeviceMemory {
	struct HostVisible {
		Backend::Devices* mpDevices{};
		VkDeviceMemory mpHostVisibleMemory{};
		std::vector<VkBuffer> mHostVisiblepBuffers{};
		std::vector<Common::BufferInfo> mHostVisibleBufferInfos{};
		std::vector<VkDeviceSize> mBufferOffsets{};
		std::vector<VkDeviceSize> mBufferSizes{};

		VkDescriptorPool mpDescriptorPool{};
		std::vector<Common::DescriptorSetInfo> mDeviceLocalDescriptorSetInfos{};
		std::vector<VkDescriptorSetLayout> mDescriptorpSetLayouts{};
		std::vector<VkDescriptorSet> mDescriptorpSets{};

		void writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES);
		void createDescriptorSet(Common::DescriptorSetInfo const& INFO, size_t const& INDEX);
		void updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);

		HostVisible();
		explicit HostVisible(Backend::Devices* pGivenDevices, const DeviceMemory::Common::HostVisibleConstructArguements (*const pCONSTRUCT_FUNCTION)(), void (*const pPOPULATE_FUNCTION)(DeviceMemory::HostVisible& toBePopulated));
		~HostVisible();

		DELETE_COPY_CONSTRUCTORS(HostVisible)
		DELETE_MOVE_CONSTRUCTORS(HostVisible)
	};
}
