#pragma once

#include "Devices.hpp"
#include "Common.h"
#include "DeviceMemoryCommon.h"

namespace DeviceMemory {
	struct HostVisible {
		struct CreateInfo {
			std::vector<Common::BufferInfo> bufferInfos{};
			std::vector<Common::DescriptorSetInfo> descriptorSetInfos{};

			CreateInfo() :
				bufferInfos{},
				descriptorSetInfos{} {}
			CreateInfo(std::vector<Common::BufferInfo> const& GIVEN_BUFFER_INFOS, std::vector<Common::DescriptorSetInfo> const& GIVEN_DESCRIPTOR_SET_INFOS) :
				bufferInfos{GIVEN_BUFFER_INFOS},
				descriptorSetInfos{GIVEN_DESCRIPTOR_SET_INFOS} {}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				bufferInfos{ std::move(salvageCreateInfo.bufferInfos) },
				descriptorSetInfos{ std::move(salvageCreateInfo.descriptorSetInfos) } {}
			DELETE_COPY_CONSTRUCTORS(CreateInfo)
		};

		Backend::Devices* devices{};
		VkDeviceMemory hostVisibleMemory{};

		const CreateInfo CREATE_INFO{};

		std::vector<VkBuffer> buffers{};
		std::vector<VkDeviceSize> bufferOffsets{};
		std::vector<VkDeviceSize> bufferSizes{};

		VkDescriptorPool descriptorPool{};
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
		std::vector<VkDescriptorSet> descriptorSets{};

		void writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES);
		void createDescriptorSetAndLayout(Common::DescriptorSetInfo const& INFO, size_t const& INDEX);
		void updateDescriptorSetBuffer(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);

		explicit HostVisible(Backend::Devices* givenDevices, CreateInfo&& givenCreateInfo, void (*const populate)(DeviceMemory::HostVisible& self));
		~HostVisible();

		DELETE_COPY_CONSTRUCTORS(HostVisible)
		DELETE_MOVE_CONSTRUCTORS(HostVisible)
	};
}
