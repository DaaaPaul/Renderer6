#pragma once

#include <functional>
#include "Devices.hpp"
#include "Common.h"
#include "DeviceMemory.h"

namespace DeviceMemory {
	class DeviceLocal {
		public:
		struct CreateInfo {
			std::vector<BufferInfo> bufferInfos{};
			std::vector<ImageInfo> imageInfos{};
			std::vector<SamplerInfo> samplerInfos{};
			std::vector<DescriptorSetInfo> descriptorSetInfos{};

			CreateInfo(std::vector<BufferInfo>&& salvageBufferInfos, std::vector<ImageInfo>&& salvageImageInfos, std::vector<SamplerInfo>&& salvageSamplerInfos, std::vector<DescriptorSetInfo>&& salvageDescriptorSetInfos) :
				bufferInfos{ std::move(salvageBufferInfos) },
				imageInfos{ std::move(salvageImageInfos) },
				samplerInfos{ std::move(salvageSamplerInfos) },
				descriptorSetInfos{ std::move(salvageDescriptorSetInfos) } {}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				bufferInfos{ std::move(salvageCreateInfo.bufferInfos) },
				imageInfos{ std::move(salvageCreateInfo.imageInfos) },
				samplerInfos{ std::move(salvageCreateInfo.samplerInfos) },
				descriptorSetInfos{ std::move(salvageCreateInfo.descriptorSetInfos) } {}
		};

		private:
		Backend::Devices* pDevices{};
		VkDeviceMemory pDeviceLocalMemory{};

		CreateInfo createInfo;
		const std::function<void(DeviceLocal&)> POPULATE;

		std::vector<VkBuffer> buffers{};
		std::vector<VkDeviceSize> bufferOffsets{};
		std::vector<VkDeviceSize> bufferSizes{};

		std::vector<VkImage> images{};
		std::vector<VkImageView> imageViews{};
		std::vector<VkDeviceSize> imageOffsets{};
		std::vector<VkDeviceSize> imageSizes{};

		std::vector<VkSampler> samplers{};

		VkDescriptorPool pDescriptorPool{};
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
		std::vector<VkDescriptorSet> descriptorSets{};

		void createBuffers();
		void createImages();
		void createMemoryAndBind();
		void createImageViews();
		void createSamplers();
		void createDescriptorSets();

		//void generateImageMipmaps(size_t const& IMAGE_INDEX);

		void recreateMemory();
		void recreateImageViews();

		public:
		[[nodiscard]] Backend::Devices*& getDevices() { return pDevices; }
		[[nodiscard]] CreateInfo& getCreateInfo() { return createInfo; }
		[[nodiscard]] std::vector<VkBuffer>& getBuffers() { return buffers; }
		[[nodiscard]] std::vector<VkImage>& getImages() { return images; }
		[[nodiscard]] std::vector<VkImageView>& getImageViews() { return imageViews; }
		[[nodiscard]] std::vector<VkDescriptorSetLayout>& getDescriptorSetLayouts() { return descriptorSetLayouts; }
		[[nodiscard]] std::vector<VkDescriptorSet>& getDescriptorSets() { return descriptorSets; }

		void copyBufferToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS);
		void copyBufferToImage(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferImageCopy> const& COPY_REGIONS);
		void createDescriptorSetAndLayout(DescriptorSetInfo const& INFO, size_t const& INDEX);
		void updateDescriptorSetBuffer(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES);
		void updateDescriptorSetCombinedImageSampler(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& SAMPLER_IMAGE_INDICES);
		void recreateDepthResources();
		[[nodiscard]] int searchForDepthImageIndex() const noexcept;

		explicit DeviceLocal(Backend::Devices* pGivenDevices, CreateInfo&& givenCreateInfo, std::function<void(DeviceLocal&)> const& POPULATE_FUNCTION);
		~DeviceLocal();

		DELETE_COPY_CONSTRUCTORS(DeviceLocal)
		DELETE_MOVE_CONSTRUCTORS(DeviceLocal)
	};
}