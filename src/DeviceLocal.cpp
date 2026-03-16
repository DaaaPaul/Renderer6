#include <iostream>
#include "DeviceLocal.hpp"
#include "Global.h"

namespace DeviceMemory {
	void DeviceLocal::createBuffers() {
		if(!createInfo.bufferInfos.empty()) {
			const uint16_t BUFFER_COUNT = createInfo.bufferInfos.size();

			buffers.resize(BUFFER_COUNT, VK_NULL_HANDLE);
			bufferSizes.resize(BUFFER_COUNT, 0);
			bufferOffsets.resize(BUFFER_COUNT, 0);
			bufferPointers.resize(BUFFER_COUNT, 0);

			for (int i = 0; i < BUFFER_COUNT; i++) {
				buffers[i] = createBuffer(pDevices->getLogicalDevice(), createInfo.bufferInfos[i]);
			}
		}
	}

	//void DeviceLocal::generateImageMipmaps(size_t const& IMAGE_INDEX) {
	//	uint16_t mipWidth = createInfo.imageInfos[IMAGE_INDEX].extent.width;
	//	uint16_t mipHeight = createInfo.imageInfos[IMAGE_INDEX].extent.height;

	//	VkCommandPool tempCommandPool{};
	//	VkCommandBuffer tempCommandBuffer{};

	//	createBeginOneTimeCommandBuffer(pDevices->getLogicalDevice(), tempCommandPool, tempCommandBuffer, pDevices->getGraphicsQfIndex());

	//	// assume mipLevelsCount includes the original image
	//	size_t dstMipLevel = 1;
	//	size_t srcMipLevel = 0;
	//	for(size_t i = 1; i < createInfo.imageInfos[IMAGE_INDEX].mipLevelsCount; i++) {
	//		dstMipLevel = i;
	//		srcMipLevel = i - 1;

	//		// transfer image srcMipLevel to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
	//		transitionImageLayout(tempCommandBuffer, images[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, srcMipLevel, 1, 0, 1), 
	//			VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, 
	//			VK_PIPELINE_STAGE_2_BLIT_BIT, VK_ACCESS_2_TRANSFER_READ_BIT, 
	//			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pDevices->getGraphicsQfIndex());

	//		VkImageBlit blit{
	//			.srcSubresource = VkImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, srcMipLevel, 0, 1),
	//			.srcOffsets = { VkOffset3D(0, 0, 0), VkOffset3D(mipWidth, mipHeight, 1) },
	//			.dstSubresource = VkImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, dstMipLevel, 0, 1),
	//			.dstOffsets = { VkOffset3D(0, 0, 0), VkOffset3D((mipWidth > 1) ? mipWidth / 2 : 1, (mipHeight > 1) ? mipHeight / 2 : 1, 1) }
	//		};
	//		vkCmdBlitImage(tempCommandBuffer, images[IMAGE_INDEX], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, images[IMAGE_INDEX], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
	//	
	//		// transfer image srcMipLevel to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	//		transitionImageLayout(tempCommandBuffer, images[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, srcMipLevel, 1, 0, 1), 
	//			VK_PIPELINE_STAGE_2_BLIT_BIT, VK_ACCESS_2_TRANSFER_READ_BIT, 
	//			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT, 
	//			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, pDevices->getGraphicsQfIndex());

	//		if(mipWidth > 1) {
	//			mipWidth /= 2;
	//		}
	//		if(mipHeight > 1) {
	//			mipHeight /= 2;
	//		}
	//	}

	//	// transfer last mip level to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	//	transitionImageLayout(tempCommandBuffer, images[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, createInfo.imageInfos[IMAGE_INDEX].mipLevelsCount - 1, 1, 0, 1), 
	//		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, 
	//		VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT, 
	//		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, pDevices->getGraphicsQfIndex());

	//	endSubmitDestroyOneTimeCommandBuffer(pDevices->getLogicalDevice(), pDevices->getGraphicsQueues()[0], tempCommandPool, tempCommandBuffer);
	//}

	void DeviceLocal::createImages() {
		if(!createInfo.imageInfos.empty()) {
			const uint16_t IMAGE_COUNT = createInfo.imageInfos.size();

			images.resize(IMAGE_COUNT, VK_NULL_HANDLE);
			imageOffsets.resize(IMAGE_COUNT, 0);
			imageSizes.resize(IMAGE_COUNT, 0);

			for(int i = 0; i < IMAGE_COUNT; i++) {
				images[i] = createImage(pDevices->getLogicalDevice(), createInfo.imageInfos[i]);
			}
		}
	}

	void DeviceLocal::createMemoryAndBind() {
		const uint16_t BUFFER_COUNT = createInfo.bufferInfos.size();
		std::vector<VkMemoryRequirements> bufferRequirements(BUFFER_COUNT, {});
		for (int i = 0; i < BUFFER_COUNT; i++) {
			vkGetBufferMemoryRequirements(pDevices->getLogicalDevice(), buffers[i], &bufferRequirements[i]);
			bufferSizes[i] = bufferRequirements[i].size;
		}

		const uint16_t IMAGE_COUNT = createInfo.imageInfos.size();
		std::vector<VkMemoryRequirements> imageRequirements(IMAGE_COUNT, {});
		for (int i = 0; i < IMAGE_COUNT; i++) {
			vkGetImageMemoryRequirements(pDevices->getLogicalDevice(), images[i], &imageRequirements[i]);
			imageSizes[i] = imageRequirements[i].size;
		}

		std::vector<VkMemoryRequirements> buffersImagesRequirements(bufferRequirements);
		buffersImagesRequirements.insert(buffersImagesRequirements.end(), imageRequirements.begin(), imageRequirements.end());

		// create the memory
		VkDeviceSize memorySize = getMemoryAllocationSizeAndOffsets(buffersImagesRequirements).first;
		std::vector<VkDeviceSize> memoryOffsets(getMemoryAllocationSizeAndOffsets(buffersImagesRequirements).second);
		uint32_t memoryType = getMemoryTypeIndex(pDevices->getPhysicalDevice(), buffersImagesRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VkMemoryAllocateFlagsInfo addressFlag{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
			.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
		};
		VkMemoryAllocateInfo allocateInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = &addressFlag,
			.allocationSize = memorySize,
			.memoryTypeIndex = memoryType
		};
		vkAllocateMemory(pDevices->getLogicalDevice(), &allocateInfo, nullptr, &pDeviceLocalMemory);

		// bind buffers and images
		bufferOffsets.assign(memoryOffsets.begin(), memoryOffsets.begin() + bufferRequirements.size());
		imageOffsets.assign(memoryOffsets.begin() + bufferRequirements.size(), memoryOffsets.end());

		for(int i = 0; i < BUFFER_COUNT; i++) {
			vkBindBufferMemory(pDevices->getLogicalDevice(), buffers[i], pDeviceLocalMemory, bufferOffsets[i]);
		}
		for(int i = 0; i < IMAGE_COUNT; i++) {
			vkBindImageMemory(pDevices->getLogicalDevice(), images[i], pDeviceLocalMemory, imageOffsets[i]);
		}

		// get buffer addresses
		VkBufferDeviceAddressInfo rollingBuffer{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
		for(int i = 0; i < BUFFER_COUNT; i++) {
			if(createInfo.bufferInfos[i].usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
				rollingBuffer.buffer = buffers[i];
				bufferPointers[i] = vkGetBufferDeviceAddress(pDevices->getLogicalDevice(), &rollingBuffer);
				std::cout << "Buffer " << i << " address: " << bufferPointers[i] << "\n";
			}
		}
	}

	void DeviceLocal::createImageViews() {
		const uint16_t IMAGE_COUNT = createInfo.imageInfos.size();

		imageViews.resize(IMAGE_COUNT, VK_NULL_HANDLE);
		for(int i = 0; i < IMAGE_COUNT; i++) {
			if(createInfo.imageInfos[i].viewInfo != ImageViewInfo{}) {
				imageViews[i] = createImageView(pDevices->getLogicalDevice(), images[i], createInfo.imageInfos[i].viewInfo);
			}
		}
	}

	void DeviceLocal::createSamplers() {
		if(!createInfo.samplerInfos.empty()) {
			const uint16_t SAMPLER_COUNT = createInfo.samplerInfos.size();

			samplers.resize(SAMPLER_COUNT, VK_NULL_HANDLE);

			for(int i = 0; i < SAMPLER_COUNT; i++) {
				samplers[i] = createSampler(pDevices->getLogicalDevice(), createInfo.samplerInfos[i]);
			}
		}
	}

	void DeviceLocal::createDescriptorSets() {
		if(!createInfo.descriptorSetInfos.empty()) {
			pDescriptorPool = createDescriptorPool(pDevices->getLogicalDevice(), createInfo.descriptorSetInfos);

			descriptorSetLayouts.resize(createInfo.descriptorSetInfos.size(), VK_NULL_HANDLE);
			descriptorSets.resize(createInfo.descriptorSetInfos.size(), VK_NULL_HANDLE);

			for(size_t i = 0; i < createInfo.descriptorSetInfos.size(); i++) {
				createDescriptorSetAndLayout(createInfo.descriptorSetInfos[i], i);
			}
		}
	}

	DeviceLocal::DeviceLocal(Backend::Devices* pGivenDevices, CreateInfo&& givenCreateInfo, std::function<void(DeviceLocal&)> const& POPULATE_FUNCTION) : 
		pDevices{ pGivenDevices },
		pDeviceLocalMemory{},
		createInfo(std::move(givenCreateInfo)),
		POPULATE(POPULATE_FUNCTION),
		buffers{},
		bufferOffsets{},
		bufferSizes{},
		bufferPointers{},
		samplers{},
		images{},
		imageViews{},
		imageOffsets{},
		imageSizes{},
		pDescriptorPool{}, 
		descriptorSetLayouts{},
		descriptorSets{} {

		createBuffers();
		createImages();
		createMemoryAndBind();
		createImageViews();
		createSamplers();
		createDescriptorSets();

		POPULATE(*this);
	}

	DeviceLocal::~DeviceLocal() {
		vkFreeMemory(pDevices->getLogicalDevice(), pDeviceLocalMemory, nullptr);
		for(VkBuffer& buffer : buffers) {
			vkDestroyBuffer(pDevices->getLogicalDevice(), buffer, nullptr);
		}
		for(VkImage& image : images) {
			vkDestroyImage(pDevices->getLogicalDevice(), image, nullptr);
		}
		for(VkImageView& imageView : imageViews) {
			vkDestroyImageView(pDevices->getLogicalDevice(), imageView, nullptr);
		}
		for(VkSampler& sampler : samplers) {
			vkDestroySampler(pDevices->getLogicalDevice(), sampler, nullptr);
		}

		for(size_t i = 0; i < descriptorSets.size(); i++) {
			vkFreeDescriptorSets(pDevices->getLogicalDevice(), pDescriptorPool, 1, &descriptorSets[i]);
			vkDestroyDescriptorSetLayout(pDevices->getLogicalDevice(), descriptorSetLayouts[i], nullptr);
		}
		if(pDescriptorPool) {
			vkDestroyDescriptorPool(pDevices->getLogicalDevice(), pDescriptorPool, nullptr);
		}
	}

	void DeviceLocal::copyBufferToBuffer(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		createBeginOneTimeCommandBuffer(pDevices->getLogicalDevice(), tempCommandPool, tempCommandBuffer, pDevices->getGraphicsQfIndex());
		vkCmdCopyBuffer(tempCommandBuffer, SRC_BUFFER, buffers[INDEX], static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());
		endSubmitDestroyOneTimeCommandBuffer(pDevices->getLogicalDevice(), pDevices->getGraphicsQueues()[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::copyBufferToImage(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferImageCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		createBeginOneTimeCommandBuffer(pDevices->getLogicalDevice(), tempCommandPool, tempCommandBuffer, pDevices->getGraphicsQfIndex());
		
		// recorded commands
		transitionImageLayout(tempCommandBuffer, images[INDEX],
		VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE, 
		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, pDevices->getGraphicsQfIndex());

		vkCmdCopyBufferToImage(tempCommandBuffer, SRC_BUFFER, images[INDEX], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());
		
		transitionImageLayout(tempCommandBuffer, images[INDEX],
		VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, 
		VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,	
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, pDevices->getGraphicsQfIndex());
		// end of recorded commands

		endSubmitDestroyOneTimeCommandBuffer(pDevices->getLogicalDevice(), pDevices->getGraphicsQueues()[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::createDescriptorSetAndLayout(DescriptorSetInfo const& INFO, size_t const& INDEX) {
		const VkDescriptorSetLayoutCreateInfo DESCRIPTOR_SET_LAYOUT_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(INFO.layoutBindings.size()),
			.pBindings = INFO.layoutBindings.data(),
		};

		CHECK_VK_SUCCESS(
			vkCreateDescriptorSetLayout(pDevices->getLogicalDevice(), &DESCRIPTOR_SET_LAYOUT_INFO, nullptr, &descriptorSetLayouts[INDEX]),
			"Failed to create descriptor set layout"
		)

		const VkDescriptorSetAllocateInfo DESCRIPTOR_SET_ALLOCATE_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = pDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &descriptorSetLayouts[INDEX],
		};

		CHECK_VK_SUCCESS(
			vkAllocateDescriptorSets(pDevices->getLogicalDevice(), &DESCRIPTOR_SET_ALLOCATE_INFO, &descriptorSets[INDEX]),
			"Failed to create descriptor set"
		)
	}

	void DeviceLocal::updateDescriptorSetBuffer(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_INDICES) {
		if(BUFFER_INDICES.size() != createInfo.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorCount) {
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
			.descriptorCount = createInfo.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorCount,
			.descriptorType = createInfo.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorType,
			.pBufferInfo = toWriteBuffers.data()
		};

		vkUpdateDescriptorSets(pDevices->getLogicalDevice(), 1, &WRITE_INFO, 0, nullptr);
	}

	void DeviceLocal::updateDescriptorSetCombinedImageSampler(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& SAMPLER_IMAGE_INDICES) {
		if(SAMPLER_IMAGE_INDICES.size() != createInfo.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorCount) {
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
			.descriptorCount = createInfo.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorCount,
			.descriptorType = createInfo.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorType,
			.pImageInfo = toWriteImageSamplers.data()
		};

		vkUpdateDescriptorSets(pDevices->getLogicalDevice(), 1, &WRITE_INFO, 0, nullptr);
	}

	void DeviceLocal::recreateMemory() {
		vkFreeMemory(pDevices->getLogicalDevice(), pDeviceLocalMemory, nullptr);
		for(VkBuffer& buffer : buffers) {
			vkDestroyBuffer(pDevices->getLogicalDevice(), buffer, nullptr);
		}
		for(VkImage& image : images) {
			vkDestroyImage(pDevices->getLogicalDevice(), image, nullptr);
		}

		createBuffers();
		createImages();
		createMemoryAndBind();
	}

	void DeviceLocal::recreateImageViews() {
		for(VkImageView& imageView : imageViews) {
			vkDestroyImageView(pDevices->getLogicalDevice(), imageView, nullptr);
		}
		createImageViews();
	}

	void DeviceLocal::recreateDepthResources() {
		const int DEPTH_INDEX = searchForDepthImageIndex();

		if(DEPTH_INDEX != -1) {
			createInfo.imageInfos[DEPTH_INDEX].extent.width = Global::getSwapchain().getCurrentExtent().width;
			createInfo.imageInfos[DEPTH_INDEX].extent.height = Global::getSwapchain().getCurrentExtent().height;

			recreateMemory();
			recreateImageViews();

			POPULATE(*this);
		} else {
			throw std::runtime_error("No depth image found!");
		}
	}

	[[nodiscard]] int DeviceLocal::searchForDepthImageIndex() const noexcept {
		int depthImageIndex = -1;
		for(int i = 0; i < createInfo.imageInfos.size() && depthImageIndex == -1; i++) {
			if(createInfo.imageInfos[i].usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
				depthImageIndex = i;
			}
		}

		return depthImageIndex;
	}
}
