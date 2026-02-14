#include "DeviceLocal.hpp"
#include <iostream>

namespace DeviceMemory {
	DeviceLocal::DeviceLocal() :
		mpDevices{},
		mpDeviceLocalMemory{},
		mBufferInfos{},
		mpBuffers{},
		mBufferOffsets{},
		mBufferSizes{}, 
		mpImages{},
		mpDescriptorPool{}, 
		mDescriptorSetInfos{},
		mDescriptorpSetLayouts{},
		mDescriptorpSets{} {}

	DeviceLocal::DeviceLocal(Backend::Devices* pGivenDevices, const DeviceMemory::Common::DeviceLocalConstructArguements (*const pCONSTRUCT_FUNCTION)(), void (*const pPOPULATE_FUNCTION)(DeviceMemory::DeviceLocal& toBePopulated)) : 
		mpDevices{ pGivenDevices },
		mpDeviceLocalMemory{},
		mpBuffers{},
		mBufferInfos{ pCONSTRUCT_FUNCTION().mBufferInfos },
		mBufferOffsets{},
		mBufferSizes{}, 
		mpImages{},
		mImageInfos{ pCONSTRUCT_FUNCTION().mImageInfos },
		mImageOffsets{},
		mImageSizes{},
		mpDescriptorPool{}, 
		mDescriptorSetInfos{ pCONSTRUCT_FUNCTION().mDescriptorSetInfos },
		mDescriptorpSetLayouts{},
		mDescriptorpSets{} {

		// memory and buffer and image stuff
		{
			// create the buffers themselves with their memory requirements info
			const size_t BUFFERS_COUNT{ mBufferInfos.size() };

			mpBuffers.resize(BUFFERS_COUNT, VK_NULL_HANDLE);
			mBufferSizes.resize(BUFFERS_COUNT, 0);
			std::vector<VkMemoryRequirements> buffersMemoryRequirements(BUFFERS_COUNT, {});

			for (int i = 0; i < BUFFERS_COUNT; i++) {
				mpBuffers[i] = Common::fCreateBuffer(mpDevices->mpLogicalDevice, mBufferInfos[i]);
				vkGetBufferMemoryRequirements(mpDevices->mpLogicalDevice, mpBuffers[i], &buffersMemoryRequirements[i]);
				mBufferSizes[i] = buffersMemoryRequirements[i].size;
			}

			// create the images themselves with their memory requirements info
			const size_t IMAGES_COUNT{ mImageInfos.size() };

			mpImages.resize(IMAGES_COUNT, VK_NULL_HANDLE);
			mImageSizes.resize(IMAGES_COUNT, 0);
			std::vector<VkMemoryRequirements> imagesMemoryRequirements(IMAGES_COUNT, {});

			VkImageCreateInfo rollingImageCreateInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.arrayLayers = 1,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 1
			};
			for(int i = 0; i < IMAGES_COUNT; i++) {
				rollingImageCreateInfo.imageType = mImageInfos[i].mImageType;
				rollingImageCreateInfo.format = mImageInfos[i].mFormat;
				rollingImageCreateInfo.extent = mImageInfos[i].mExtent3D;
				rollingImageCreateInfo.mipLevels = mImageInfos[i].mMipLevels;
				rollingImageCreateInfo.samples = mImageInfos[i].mSampleCount;
				rollingImageCreateInfo.usage = mImageInfos[i].mUsage;
				rollingImageCreateInfo.pQueueFamilyIndices = &mImageInfos[i].mGraphicsQueueFamilyIndex;
				rollingImageCreateInfo.initialLayout = mImageInfos[i].mInitialLayout;

				CHECK_VK_SUCCESS(
					vkCreateImage(mpDevices->mpLogicalDevice, &rollingImageCreateInfo, nullptr, &mpImages[i]),
					"Failed to create VkImage"
				)

				vkGetImageMemoryRequirements(mpDevices->mpLogicalDevice, mpImages[i], &imagesMemoryRequirements[i]);
				mImageSizes[i] = imagesMemoryRequirements[i].size;
			}

			std::vector<VkMemoryRequirements> allMemoryRequirements{};
			for(VkMemoryRequirements const& BUFFER_MEM_REQUIREMENTS : buffersMemoryRequirements) {
				allMemoryRequirements.push_back(BUFFER_MEM_REQUIREMENTS);
			}
			for(VkMemoryRequirements const& IMAGE_MEM_REQUIREMENTS : imagesMemoryRequirements) {
				allMemoryRequirements.push_back(IMAGE_MEM_REQUIREMENTS);
			}

			// create the memory
			VkMemoryAllocateInfo hostVisibleMemoryAllocateInfo{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.allocationSize = Common::fGetMemoryAllocationSizeAndOffsets(allMemoryRequirements).first,
				.memoryTypeIndex = Common::fGetMemoryTypeIndex(mpDevices->mpPhysicalDevice, allMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			};
			vkAllocateMemory(mpDevices->mpLogicalDevice, &hostVisibleMemoryAllocateInfo, nullptr, &mpDeviceLocalMemory);

			// bind buffers and images
			const std::vector<VkDeviceSize> MEMORY_OFFSETS{ Common::fGetMemoryAllocationSizeAndOffsets(allMemoryRequirements).second };
			mBufferOffsets.assign(MEMORY_OFFSETS.begin(), MEMORY_OFFSETS.begin() + buffersMemoryRequirements.size());
			mImageOffsets.assign(MEMORY_OFFSETS.begin() + buffersMemoryRequirements.size(), MEMORY_OFFSETS.end());

			for(int i = 0; i < BUFFERS_COUNT; i++) {
				vkBindBufferMemory(mpDevices->mpLogicalDevice, mpBuffers[i], mpDeviceLocalMemory, mBufferOffsets[i]);
			}
			for(int i = 0; i < IMAGES_COUNT; i++) {
				vkBindImageMemory(mpDevices->mpLogicalDevice, mpImages[i], mpDeviceLocalMemory, mImageOffsets[i]);
			}
		}

		// descriptor set stuff
		if(!mDescriptorSetInfos.empty()) {
			mpDescriptorPool = Common::fCreateDescriptorPool(mpDevices->mpLogicalDevice, mDescriptorSetInfos);

			// create the descriptor sets
			mDescriptorpSetLayouts.resize(mDescriptorSetInfos.size(), VK_NULL_HANDLE);
			mDescriptorpSets.resize(mDescriptorSetInfos.size(), VK_NULL_HANDLE);

			for(size_t i = 0; i < mDescriptorSetInfos.size(); i++) {
				createDescriptorSet(mDescriptorSetInfos[i], i);
			}
		}

		pPOPULATE_FUNCTION(*this);
	}

	DeviceLocal::~DeviceLocal() {
		vkFreeMemory(mpDevices->mpLogicalDevice, mpDeviceLocalMemory, nullptr);
		for(VkBuffer& buffer : mpBuffers) {
			vkDestroyBuffer(mpDevices->mpLogicalDevice, buffer, nullptr);
		}
		for(VkImage& image : mpImages) {
			vkDestroyImage(mpDevices->mpLogicalDevice, image, nullptr);
		}

		for(size_t i = 0; i < mDescriptorpSets.size(); i++) {
			vkFreeDescriptorSets(mpDevices->mpLogicalDevice, mpDescriptorPool, 1, &mDescriptorpSets[i]);
			vkDestroyDescriptorSetLayout(mpDevices->mpLogicalDevice, mDescriptorpSetLayouts[i], nullptr);
		}
		if(mpDescriptorPool) {
			vkDestroyDescriptorPool(mpDevices->mpLogicalDevice, mpDescriptorPool, nullptr);
		}
	}

	void DeviceLocal::copyBufferToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		Common::fAllocateBeginOneTimeCommandBuffer(mpDevices->mpLogicalDevice, tempCommandPool, tempCommandBuffer, mpDevices->mGRAPHICS_QUEUE_FAMILY_INDEX);
		vkCmdCopyBuffer(tempCommandBuffer, SRC_BUFFER, mpBuffers[INDEX], static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());
		Common::fEndSubmitDeallocateOneTimeCommandBuffer(mpDevices->mpLogicalDevice, mpDevices->mGraphicsFamilypQueues[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::copyBufferToImage(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferImageCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		Common::fAllocateBeginOneTimeCommandBuffer(mpDevices->mpLogicalDevice, tempCommandPool, tempCommandBuffer, mpDevices->mGRAPHICS_QUEUE_FAMILY_INDEX);
		
		// recorded commands
		Common::fTransitionImageLayout(tempCommandBuffer, mpImages[INDEX],
		VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE, 
		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_NONE, // none because if srcAccess is none, then there is no point of dstAccess
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mpDevices->mGRAPHICS_QUEUE_FAMILY_INDEX);

		vkCmdCopyBufferToImage(tempCommandBuffer, SRC_BUFFER, mpImages[INDEX], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());

		Common::fTransitionImageLayout(tempCommandBuffer, mpImages[INDEX],
		VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, 
		VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mpDevices->mGRAPHICS_QUEUE_FAMILY_INDEX);
		// end of recorded commands

		Common::fEndSubmitDeallocateOneTimeCommandBuffer(mpDevices->mpLogicalDevice, mpDevices->mGraphicsFamilypQueues[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::createDescriptorSet(Common::DescriptorSetInfo const& INFO, size_t const& INDEX) {
		const VkDescriptorSetLayoutCreateInfo DESCRIPTOR_SET_LAYOUT_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(INFO.mLayoutBindings.size()),
			.pBindings = INFO.mLayoutBindings.data(),
		};

		CHECK_VK_SUCCESS(
			vkCreateDescriptorSetLayout(mpDevices->mpLogicalDevice, &DESCRIPTOR_SET_LAYOUT_INFO, nullptr, &mDescriptorpSetLayouts[INDEX]),
			"Failed to create descriptor set layout"
		)

		const VkDescriptorSetAllocateInfo DESCRIPTOR_SET_ALLOCATE_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = mpDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &mDescriptorpSetLayouts[INDEX],
		};

		CHECK_VK_SUCCESS(
			vkAllocateDescriptorSets(mpDevices->mpLogicalDevice, &DESCRIPTOR_SET_ALLOCATE_INFO, &mDescriptorpSets[INDEX]),
			"Failed to create descriptor set"
		)
	}

	void DeviceLocal::updateDescriptorSet(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES) {
		if(BUFFER_INDICES.size() != mDescriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorCount) {
			throw std::runtime_error("Number of buffers must match number of descriptors in set " + std::to_string(SET_INDEX) + " binding " + std::to_string(SET_BINDING_NUM));
		}
	
		std::vector<VkDescriptorBufferInfo> toWriteBuffers{};
		for(size_t const& BUFFER_INDEX : BUFFER_INDICES) {
			toWriteBuffers.emplace_back(mpBuffers[BUFFER_INDEX], 0, VK_WHOLE_SIZE);
		}
	
		const VkWriteDescriptorSet WRITE_INFO{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = mDescriptorpSets[SET_INDEX],
			.dstBinding = SET_BINDING_NUM,
			.dstArrayElement = 0,
			.descriptorCount = mDescriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorCount,
			.descriptorType = mDescriptorSetInfos[SET_INDEX].mLayoutBindings[SET_BINDING_NUM].descriptorType,
			.pBufferInfo = toWriteBuffers.data()
		};

		vkUpdateDescriptorSets(mpDevices->mpLogicalDevice, 1, &WRITE_INFO, 0, nullptr);
	}
}
