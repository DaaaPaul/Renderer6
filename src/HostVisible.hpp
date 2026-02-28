#pragma once

#include "Devices.hpp"
#include "Common.h"
#include "DeviceMemoryCommon.h"

namespace DeviceMemory {
	class HostVisible {
		public:
		struct CreateInfo {
			std::vector<Common::BufferInfo> bufferInfos{};
			std::vector<Common::DescriptorSetInfo> descriptorSetInfos{};

			CreateInfo(std::vector<Common::BufferInfo>&& salvageBufferInfos, std::vector<Common::DescriptorSetInfo>&& salvageDescriptorSetInfos) :
				bufferInfos{ std::move(salvageBufferInfos) },
				descriptorSetInfos{ std::move(salvageDescriptorSetInfos) } {}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				bufferInfos{ std::move(salvageCreateInfo.bufferInfos) },
				descriptorSetInfos{ std::move(salvageCreateInfo.descriptorSetInfos) } {}
			DELETE_COPY_CONSTRUCTORS(CreateInfo)
		};

		private:
		Backend::Devices* devices{};
		VkDeviceMemory hostVisibleMemory{};

		const CreateInfo CREATE_INFO;

		std::vector<VkBuffer> buffers{};
		std::vector<VkDeviceSize> bufferOffsets{};
		std::vector<VkDeviceSize> bufferSizes{};

		VkDescriptorPool descriptorPool{};
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
		std::vector<VkDescriptorSet> descriptorSets{};

		public:
		[[nodiscard]] Backend::Devices*& getDevices() { return devices; }
		[[nodiscard]] CreateInfo const& getCreateInfo() const { return CREATE_INFO; }
		[[nodiscard]] std::vector<VkBuffer>& getBuffers() { return buffers; }
		[[nodiscard]] std::vector<VkDescriptorSetLayout>& getDescriptorSetLayouts() { return descriptorSetLayouts; }
		[[nodiscard]] std::vector<VkDescriptorSet>& getDescriptorSets() { return descriptorSets; }

		void writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES);
		void createDescriptorSetAndLayout(Common::DescriptorSetInfo const& INFO, size_t const& INDEX);
		void updateDescriptorSetBuffer(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);

		explicit HostVisible(Backend::Devices* givenDevices, CreateInfo&& givenCreateInfo, void (*const populate)(DeviceMemory::HostVisible& self));
		~HostVisible();

		DELETE_COPY_CONSTRUCTORS(HostVisible)
		DELETE_MOVE_CONSTRUCTORS(HostVisible)
	};
}
