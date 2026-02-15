#pragma once

#include "Devices.hpp"
#include "Common.h"
#include "DeviceMemoryCommon.h"

namespace DeviceMemory {
	struct DeviceLocal {
		Backend::Devices* mpDevices{};
		VkDeviceMemory mpDeviceLocalMemory{};

		std::vector<VkBuffer> mpBuffers{};
		std::vector<Common::BufferInfo> mBufferInfos{};
		std::vector<VkDeviceSize> mBufferOffsets{};
		std::vector<VkDeviceSize> mBufferSizes{};

		std::vector<VkImage> mpImages{};
		std::vector<VkImageView> mpImageViews{};
		std::vector<Common::ImageInfo> mImageInfos{};
		std::vector<VkDeviceSize> mImageOffsets{};
		std::vector<VkDeviceSize> mImageSizes{};

		std::vector<VkSampler> mpSamplers{};
		std::vector<Common::SamplerInfo> mSamplerInfos{};

		VkDescriptorPool mpDescriptorPool{};
		std::vector<Common::DescriptorSetInfo> mDescriptorSetInfos{};
		std::vector<VkDescriptorSetLayout> mDescriptorpSetLayouts{};
		std::vector<VkDescriptorSet> mDescriptorpSets{};

		void copyBufferToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS);
		void copyBufferToImage(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferImageCopy> const& COPY_REGIONS);
		void createDescriptorSetAndLayout(Common::DescriptorSetInfo const& INFO, size_t const& INDEX);
		void updateDescriptorSetBuffer(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);
		void updateDescriptorSetCombinedImageSampler(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& SAMPLER_IMAGE_INDICES);

		DeviceLocal();
		explicit DeviceLocal(Backend::Devices* pGivenDevices, const DeviceMemory::Common::DeviceLocalConstructArguements (*const pCONSTRUCT_FUNCTION)(), void (*const pPOPULATE_FUNCTION)(DeviceMemory::DeviceLocal& toBePopulated));
		~DeviceLocal();

		DELETE_COPY_CONSTRUCTORS(DeviceLocal)
		DELETE_MOVE_CONSTRUCTORS(DeviceLocal)
	};
}