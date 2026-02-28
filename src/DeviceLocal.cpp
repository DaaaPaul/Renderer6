#include "DeviceLocal.hpp"
#include <iostream>

namespace DeviceMemory {
	DeviceLocal::DeviceLocal(Backend::Devices* givenDevices, CreateInfo&& givenCreateInfo, void (*const populate)(DeviceMemory::DeviceLocal& self)) : 
		devices{ givenDevices },
		deviceLocalMemory{},
		CREATE_INFO(std::move(givenCreateInfo)),
		buffers{},
		bufferOffsets{},
		bufferSizes{},
		samplers{},
		images{},
		imageViews{},
		imageOffsets{},
		imageSizes{},
		descriptorPool{}, 
		descriptorSetLayouts{},
		descriptorSets{} {

		// memory and buffer and image stuff
		{
			// create the buffers themselves with their memory requirements info
			const size_t BUFFERS_COUNT{ CREATE_INFO.bufferInfos.size() };

			buffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
			bufferSizes.resize(BUFFERS_COUNT, 0);
			std::vector<VkMemoryRequirements> buffersMemoryRequirements(BUFFERS_COUNT, {});

			for (int i = 0; i < BUFFERS_COUNT; i++) {
				buffers[i] = Common::fCreateBuffer(devices->getLogicalDevice(), CREATE_INFO.bufferInfos[i]);
				vkGetBufferMemoryRequirements(devices->getLogicalDevice(), buffers[i], &buffersMemoryRequirements[i]);
				bufferSizes[i] = buffersMemoryRequirements[i].size;
			}

			// create the images themselves with their memory requirements info
			const size_t IMAGES_COUNT{ CREATE_INFO.imageInfos.size() };

			images.resize(IMAGES_COUNT, VK_NULL_HANDLE);
			imageViews.resize(IMAGES_COUNT, VK_NULL_HANDLE);
			imageSizes.resize(IMAGES_COUNT, 0);
			std::vector<VkMemoryRequirements> imagesMemoryRequirements(IMAGES_COUNT, {});

			VkImageCreateInfo rollingImageCreateInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.arrayLayers = 1,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 1
			};
			for(int i = 0; i < IMAGES_COUNT; i++) {
				rollingImageCreateInfo.imageType = CREATE_INFO.imageInfos[i].mImageType;
				rollingImageCreateInfo.format = CREATE_INFO.imageInfos[i].mFormat;
				rollingImageCreateInfo.extent = CREATE_INFO.imageInfos[i].mExtent3D;
				rollingImageCreateInfo.mipLevels = CREATE_INFO.imageInfos[i].mMipLevels;
				rollingImageCreateInfo.samples = CREATE_INFO.imageInfos[i].mSampleCount;
				rollingImageCreateInfo.usage = CREATE_INFO.imageInfos[i].mUsage;
				rollingImageCreateInfo.pQueueFamilyIndices = &CREATE_INFO.imageInfos[i].graphicsQfIndex;
				rollingImageCreateInfo.initialLayout = CREATE_INFO.imageInfos[i].mInitialLayout;

				CHECK_VK_SUCCESS(
					vkCreateImage(devices->getLogicalDevice(), &rollingImageCreateInfo, nullptr, &images[i]),
					"Failed to create VkImage"
				)

				vkGetImageMemoryRequirements(devices->getLogicalDevice(), images[i], &imagesMemoryRequirements[i]);
				imageSizes[i] = imagesMemoryRequirements[i].size;
			}

			std::vector<VkMemoryRequirements> allMemoryRequirements{};
			for(VkMemoryRequirements const& BUFFER_MEM_REQUIREMENTS : buffersMemoryRequirements) {
				allMemoryRequirements.push_back(BUFFER_MEM_REQUIREMENTS);
			}
			for(VkMemoryRequirements const& IMAGE_MEM_REQUIREMENTS : imagesMemoryRequirements) {
				allMemoryRequirements.push_back(IMAGE_MEM_REQUIREMENTS);
			}

			// create the memory
			VkMemoryAllocateInfo deviceLocalMemoryAllocateInfo{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.allocationSize = Common::fGetMemoryAllocationSizeAndOffsets(allMemoryRequirements).first,
				.memoryTypeIndex = Common::fGetMemoryTypeIndex(devices->getPhysicalDevice(), allMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			};
			vkAllocateMemory(devices->getLogicalDevice(), &deviceLocalMemoryAllocateInfo, nullptr, &deviceLocalMemory);

			// bind buffers and images
			const std::vector<VkDeviceSize> MEMORY_OFFSETS{ Common::fGetMemoryAllocationSizeAndOffsets(allMemoryRequirements).second };
			bufferOffsets.assign(MEMORY_OFFSETS.begin(), MEMORY_OFFSETS.begin() + buffersMemoryRequirements.size());
			imageOffsets.assign(MEMORY_OFFSETS.begin() + buffersMemoryRequirements.size(), MEMORY_OFFSETS.end());

			for(int i = 0; i < BUFFERS_COUNT; i++) {
				vkBindBufferMemory(devices->getLogicalDevice(), buffers[i], deviceLocalMemory, bufferOffsets[i]);
			}
			for(int i = 0; i < IMAGES_COUNT; i++) {
				vkBindImageMemory(devices->getLogicalDevice(), images[i], deviceLocalMemory, imageOffsets[i]);
			}

			// finally create image views after binding images to memory
			for(int i = 0; i < IMAGES_COUNT; i++) {
				bool hasImageView{ CREATE_INFO.imageInfos[i].mImageViewInfo.mImageViewType != 0 && CREATE_INFO.imageInfos[i].mImageViewInfo.mFormat != 0 };
				if(hasImageView) {
					VkImageViewCreateInfo rollingImageViewCreateInfo{
						.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
						.image = images[i],
						.viewType = CREATE_INFO.imageInfos[i].mImageViewInfo.mImageViewType,
						.format = CREATE_INFO.imageInfos[i].mImageViewInfo.mFormat,
						.subresourceRange = CREATE_INFO.imageInfos[i].mImageViewInfo.mImageSubresourceRange
					};

					CHECK_VK_SUCCESS(
						vkCreateImageView(devices->getLogicalDevice(), &rollingImageViewCreateInfo, nullptr, &imageViews[i]),
						"Failed to create VkImageView"
					)
				}
			}
		}

		// sampler stuff
		if(!CREATE_INFO.samplerInfos.empty()) {
			samplers.resize(CREATE_INFO.samplerInfos.size(), VK_NULL_HANDLE);

			VkSamplerCreateInfo rollingSamplerInfo{
				.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
				.unnormalizedCoordinates = VK_FALSE
			};

			for(int i = 0; i < CREATE_INFO.samplerInfos.size(); i++) {
				rollingSamplerInfo.magFilter = CREATE_INFO.samplerInfos[i].mMagFilter;
				rollingSamplerInfo.minFilter = CREATE_INFO.samplerInfos[i].mMinFilter;
				rollingSamplerInfo.mipmapMode = CREATE_INFO.samplerInfos[i].mMipmapMode;
				rollingSamplerInfo.addressModeU = CREATE_INFO.samplerInfos[i].mAddressModeU;
				rollingSamplerInfo.addressModeV = CREATE_INFO.samplerInfos[i].mAddressModeV;
				rollingSamplerInfo.mipLodBias = CREATE_INFO.samplerInfos[i].mMipLodBias;
				rollingSamplerInfo.anisotropyEnable = CREATE_INFO.samplerInfos[i].mAnisotropyEnable;
				rollingSamplerInfo.maxAnisotropy = CREATE_INFO.samplerInfos[i].mMaxAnisotropy;
				rollingSamplerInfo.minLod = CREATE_INFO.samplerInfos[i].mMinLod;
				rollingSamplerInfo.maxLod = CREATE_INFO.samplerInfos[i].mMaxLod;
				rollingSamplerInfo.borderColor = CREATE_INFO.samplerInfos[i].mBorderColor;

				CHECK_VK_SUCCESS(
					vkCreateSampler(devices->getLogicalDevice(), &rollingSamplerInfo, nullptr, &samplers[i]),
					"Failed to create sampler"
				)
			}
		}
		
		// descriptor set stuff
		if(!CREATE_INFO.descriptorSetInfos.empty()) {
			descriptorPool = Common::fCreateDescriptorPool(devices->getLogicalDevice(), CREATE_INFO.descriptorSetInfos);

			// create the descriptor sets
			descriptorSetLayouts.resize(CREATE_INFO.descriptorSetInfos.size(), VK_NULL_HANDLE);
			descriptorSets.resize(CREATE_INFO.descriptorSetInfos.size(), VK_NULL_HANDLE);

			for(size_t i = 0; i < CREATE_INFO.descriptorSetInfos.size(); i++) {
				createDescriptorSetAndLayout(CREATE_INFO.descriptorSetInfos[i], i);
			}
		}

		populate(*this);
	}

	DeviceLocal::~DeviceLocal() {
		vkFreeMemory(devices->getLogicalDevice(), deviceLocalMemory, nullptr);
		for(VkBuffer& buffer : buffers) {
			vkDestroyBuffer(devices->getLogicalDevice(), buffer, nullptr);
		}
		for(VkImageView& imageView : imageViews) {
			vkDestroyImageView(devices->getLogicalDevice(), imageView, nullptr);
		}
		for(VkImage& image : images) {
			vkDestroyImage(devices->getLogicalDevice(), image, nullptr);
		}
		for(VkSampler& sampler : samplers) {
			vkDestroySampler(devices->getLogicalDevice(), sampler, nullptr);
		}

		for(size_t i = 0; i < descriptorSets.size(); i++) {
			vkFreeDescriptorSets(devices->getLogicalDevice(), descriptorPool, 1, &descriptorSets[i]);
			vkDestroyDescriptorSetLayout(devices->getLogicalDevice(), descriptorSetLayouts[i], nullptr);
		}
		if(descriptorPool) {
			vkDestroyDescriptorPool(devices->getLogicalDevice(), descriptorPool, nullptr);
		}
	}

	void DeviceLocal::copyBufferToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		Common::fAllocateBeginOneTimeCommandBuffer(devices->getLogicalDevice(), tempCommandPool, tempCommandBuffer, devices->getGraphicsQfIndex());
		vkCmdCopyBuffer(tempCommandBuffer, SRC_BUFFER, buffers[INDEX], static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());
		Common::fEndSubmitDeallocateOneTimeCommandBuffer(devices->getLogicalDevice(), devices->getGraphicsQueues()[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::copyBufferToImage(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferImageCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		Common::fAllocateBeginOneTimeCommandBuffer(devices->getLogicalDevice(), tempCommandPool, tempCommandBuffer, devices->getGraphicsQfIndex());
		
		// recorded commands
		Common::fTransitionImageLayout(tempCommandBuffer, images[INDEX],
		VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE, 
		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, devices->getGraphicsQfIndex());

		vkCmdCopyBufferToImage(tempCommandBuffer, SRC_BUFFER, images[INDEX], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());

		Common::fTransitionImageLayout(tempCommandBuffer, images[INDEX],
		VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, 
		VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, devices->getGraphicsQfIndex());
		// end of recorded commands

		Common::fEndSubmitDeallocateOneTimeCommandBuffer(devices->getLogicalDevice(), devices->getGraphicsQueues()[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::createDescriptorSetAndLayout(Common::DescriptorSetInfo const& INFO, size_t const& INDEX) {
		const VkDescriptorSetLayoutCreateInfo DESCRIPTOR_SET_LAYOUT_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(INFO.mLayoutBindings.size()),
			.pBindings = INFO.mLayoutBindings.data(),
		};

		CHECK_VK_SUCCESS(
			vkCreateDescriptorSetLayout(devices->getLogicalDevice(), &DESCRIPTOR_SET_LAYOUT_INFO, nullptr, &descriptorSetLayouts[INDEX]),
			"Failed to create descriptor set layout"
		)

		const VkDescriptorSetAllocateInfo DESCRIPTOR_SET_ALLOCATE_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = descriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &descriptorSetLayouts[INDEX],
		};

		CHECK_VK_SUCCESS(
			vkAllocateDescriptorSets(devices->getLogicalDevice(), &DESCRIPTOR_SET_ALLOCATE_INFO, &descriptorSets[INDEX]),
			"Failed to create descriptor set"
		)
	}

	void DeviceLocal::updateDescriptorSetBuffer(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES) {
		if(BUFFER_INDICES.size() != CREATE_INFO.descriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorCount) {
			throw std::runtime_error("Number of buffers must match number of descriptors in set " + std::to_string(SET_INDEX) + " binding " + std::to_string(SET_BINDING_NUM));
		}
	
		std::vector<VkDescriptorBufferInfo> toWriteBuffers{};
		for(size_t const& BUFFER_INDEX : BUFFER_INDICES) {
			toWriteBuffers.emplace_back(buffers[BUFFER_INDEX], 0, VK_WHOLE_SIZE);
		}
	
		const VkWriteDescriptorSet WRITE_INFO{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[SET_INDEX],
			.dstBinding = SET_BINDING_NUM,
			.dstArrayElement = 0,
			.descriptorCount = CREATE_INFO.descriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorCount,
			.descriptorType = CREATE_INFO.descriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorType,
			.pBufferInfo = toWriteBuffers.data()
		};

		vkUpdateDescriptorSets(devices->getLogicalDevice(), 1, &WRITE_INFO, 0, nullptr);
	}

	void DeviceLocal::updateDescriptorSetCombinedImageSampler(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& SAMPLER_IMAGE_INDICES) {
		if(SAMPLER_IMAGE_INDICES.size() != CREATE_INFO.descriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorCount) {
			throw std::runtime_error("Number of images/samplers must match number of descriptors in set " + std::to_string(SET_INDEX) + " binding " + std::to_string(SET_BINDING_NUM));
		}

		std::vector<VkDescriptorImageInfo> toWriteImageSamplers{};
		for(size_t const& SAMPLER_IMAGE_INDEX : SAMPLER_IMAGE_INDICES) {
			toWriteImageSamplers.emplace_back(samplers[SAMPLER_IMAGE_INDEX], imageViews[SAMPLER_IMAGE_INDEX], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		const VkWriteDescriptorSet WRITE_INFO{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[SET_INDEX],
			.dstBinding = SET_BINDING_NUM,
			.dstArrayElement = 0,
			.descriptorCount = CREATE_INFO.descriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorCount,
			.descriptorType = CREATE_INFO.descriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorType,
			.pImageInfo = toWriteImageSamplers.data()
		};

		vkUpdateDescriptorSets(devices->getLogicalDevice(), 1, &WRITE_INFO, 0, nullptr);
	}

	void DeviceLocal::recreateImage(size_t const& INDEX, Common::ImageInfo const& NEW_INFO) {
		
	}
}
