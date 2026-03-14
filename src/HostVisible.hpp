#pragma once

#include <functional>
#include "Devices.hpp"
#include "Common.h"
#include "DeviceMemory.h"

namespace DeviceMemory {
	class HostVisible {
		public:
		struct CreateInfo {
			std::vector<BufferInfo> bufferInfos{};
			std::vector<DescriptorSetInfo> descriptorSetInfos{};

			CreateInfo(std::vector<BufferInfo>&& salvageBufferInfos, std::vector<DescriptorSetInfo>&& salvageDescriptorSetInfos) :
				bufferInfos{ std::move(salvageBufferInfos) },
				descriptorSetInfos{ std::move(salvageDescriptorSetInfos) } {}
			CreateInfo(CreateInfo&& salvageCreateInfo) noexcept : 
				bufferInfos{ std::move(salvageCreateInfo.bufferInfos) },
				descriptorSetInfos{ std::move(salvageCreateInfo.descriptorSetInfos) } {}
		};

		private:
		Backend::Devices* pDevices{};
		VkDeviceMemory pHostVisibleMemory{};

		const CreateInfo CREATE_INFO;

		std::vector<VkBuffer> buffers{};
		std::vector<VkDeviceSize> bufferOffsets{};
		std::vector<VkDeviceSize> bufferSizes{};

		VkDescriptorPool pDescriptorPool{};
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
		std::vector<VkDescriptorSet> descriptorSets{};

		public:
		[[nodiscard]] Backend::Devices*& getDevices() { return pDevices; }
		[[nodiscard]] CreateInfo const& getCreateInfo() const { return CREATE_INFO; }
		[[nodiscard]] std::vector<VkBuffer>& getBuffers() { return buffers; }
		[[nodiscard]] std::vector<VkDescriptorSetLayout>& getDescriptorSetLayouts() { return descriptorSetLayouts; }
		[[nodiscard]] std::vector<VkDescriptorSet>& getDescriptorSets() { return descriptorSets; }

		void writeToBuffer(size_t const& INDEX, void const*const pDATA, uint32_t const& NUM_BYTES);
		void createDescriptorSetAndLayout(DescriptorSetInfo const& INFO, size_t const& INDEX);
		void updateDescriptorSetBuffer(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);

		explicit HostVisible(Backend::Devices* pGivenDevices, CreateInfo&& givenCreateInfo, std::function<void(HostVisible&)> const& POPULATE_FUNCTION);
		~HostVisible();

		DELETE_COPY_CONSTRUCTORS(HostVisible)
		DELETE_MOVE_CONSTRUCTORS(HostVisible)
	};
}
