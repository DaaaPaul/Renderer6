#pragma once

#include "Devices.hpp"
#include "Common.h"
#include "DeviceMemoryCommon.h"

namespace DeviceMemory {
	struct DeviceLocal {
		struct CreateInfo {
			std::vector<Common::BufferInfo> bufferInfos{};
			std::vector<Common::ImageInfo> imageInfos{};
			std::vector<Common::SamplerInfo> samplerInfos{};
			std::vector<Common::DescriptorSetInfo> descriptorSetInfos{};

			CreateInfo() :
				bufferInfos{},
				imageInfos{},
				samplerInfos{},
				descriptorSetInfos{} {}
			CreateInfo(std::vector<Common::BufferInfo> const& GIVEN_BUFFER_INFOS, std::vector<Common::ImageInfo> const& GIVEN_IMAGE_INFOS, std::vector<Common::SamplerInfo> const& GIVEN_SAMPLER_INFOS, std::vector<Common::DescriptorSetInfo> const& GIVEN_DESCRIPTOR_SET_INFOS) :
				bufferInfos{GIVEN_BUFFER_INFOS},
				imageInfos{GIVEN_IMAGE_INFOS},
				samplerInfos{GIVEN_SAMPLER_INFOS},
				descriptorSetInfos{GIVEN_DESCRIPTOR_SET_INFOS} {}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				bufferInfos{ std::move(salvageCreateInfo.bufferInfos) },
				imageInfos{ std::move(salvageCreateInfo.imageInfos) },
				samplerInfos{ std::move(salvageCreateInfo.samplerInfos) },
				descriptorSetInfos{ std::move(salvageCreateInfo.descriptorSetInfos) } {}
			DELETE_COPY_CONSTRUCTORS(CreateInfo)
		};

		Backend::Devices* devices{};
		VkDeviceMemory deviceLocalMemory{};

		const CreateInfo CREATE_INFO{};

		std::vector<VkBuffer> buffers{};
		std::vector<VkDeviceSize> bufferOffsets{};
		std::vector<VkDeviceSize> bufferSizes{};

		std::vector<VkImage> images{};
		std::vector<VkImageView> imageViews{};
		std::vector<VkDeviceSize> imageOffsets{};
		std::vector<VkDeviceSize> imageSizes{};

		std::vector<VkSampler> samplers{};

		VkDescriptorPool descriptorPool{};
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
		std::vector<VkDescriptorSet> descriptorSets{};

		void copyBufferToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS);
		void copyBufferToImage(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferImageCopy> const& COPY_REGIONS);
		void createDescriptorSetAndLayout(Common::DescriptorSetInfo const& INFO, size_t const& INDEX);
		void updateDescriptorSetBuffer(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);
		void updateDescriptorSetCombinedImageSampler(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& SAMPLER_IMAGE_INDICES);
		void recreateImage(size_t const& INDEX, Common::ImageInfo const& NEW_INFO);

		explicit DeviceLocal(Backend::Devices* givenDevices, CreateInfo&& givenCreateInfo, void (*const populate)(DeviceMemory::DeviceLocal& self));
		~DeviceLocal();

		DELETE_COPY_CONSTRUCTORS(DeviceLocal)
		DELETE_MOVE_CONSTRUCTORS(DeviceLocal)
	};
}