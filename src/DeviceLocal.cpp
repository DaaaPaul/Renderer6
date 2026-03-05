#include <iostream>
#include "DeviceLocal.hpp"
#include "GlobalState.h"

namespace DeviceMemory {
	void DeviceLocal::createBuffers() {
		if(!createInfo.bufferInfos.empty()) {
			const uint16_t BUFFER_COUNT = createInfo.bufferInfos.size();

			buffers.resize(BUFFER_COUNT, VK_NULL_HANDLE);
			bufferSizes.resize(BUFFER_COUNT, 0);
			bufferOffsets.resize(BUFFER_COUNT, 0);

			for (int i = 0; i < BUFFER_COUNT; i++) {
				buffers[i] = Common::createBuffer(devices->getLogicalDevice(), createInfo.bufferInfos[i]);
			}
		}
	}

	void DeviceLocal::createImages() {
		if(!createInfo.imageInfos.empty()) {
			const uint16_t IMAGE_COUNT = createInfo.imageInfos.size();

			images.resize(IMAGE_COUNT, VK_NULL_HANDLE);
			imageOffsets.resize(IMAGE_COUNT, 0);
			imageSizes.resize(IMAGE_COUNT, 0);

			for(int i = 0; i < IMAGE_COUNT; i++) {
				images[i] = Common::createImage(devices->getLogicalDevice(), createInfo.imageInfos[i]);
			}
		}
	}

	void DeviceLocal::createMemoryAndBind() {
		const uint16_t BUFFER_COUNT = createInfo.bufferInfos.size();
		std::vector<VkMemoryRequirements> bufferRequirements(BUFFER_COUNT, {});
		for (int i = 0; i < BUFFER_COUNT; i++) {
			vkGetBufferMemoryRequirements(devices->getLogicalDevice(), buffers[i], &bufferRequirements[i]);
			bufferSizes[i] = bufferRequirements[i].size;
		}

		const uint16_t IMAGE_COUNT = createInfo.imageInfos.size();
		std::vector<VkMemoryRequirements> imageRequirements(IMAGE_COUNT, {});
		for (int i = 0; i < IMAGE_COUNT; i++) {
			vkGetImageMemoryRequirements(devices->getLogicalDevice(), images[i], &imageRequirements[i]);
			imageSizes[i] = imageRequirements[i].size;
		}

		std::vector<VkMemoryRequirements> concatenation(bufferRequirements);
		concatenation.insert(concatenation.end(), imageRequirements.begin(), imageRequirements.end());

		// create the memory
		const std::pair<VkDeviceSize, std::vector<VkDeviceSize>> SIZE_AND_OFFSETS(Common::getMemoryAllocationSizeAndOffsets(concatenation));
		const uint32_t MEMORY_TYPE = Common::getMemoryTypeIndex(devices->getPhysicalDevice(), concatenation, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VkMemoryAllocateInfo allocateInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = SIZE_AND_OFFSETS.first,
			.memoryTypeIndex = MEMORY_TYPE
		};
		vkAllocateMemory(devices->getLogicalDevice(), &allocateInfo, nullptr, &deviceLocalMemory);

		// bind buffers and images
		bufferOffsets.assign(SIZE_AND_OFFSETS.second.begin(), SIZE_AND_OFFSETS.second.begin() + bufferRequirements.size());
		imageOffsets.assign(SIZE_AND_OFFSETS.second.begin() + bufferRequirements.size(), SIZE_AND_OFFSETS.second.end());

		for(int i = 0; i < BUFFER_COUNT; i++) {
			vkBindBufferMemory(devices->getLogicalDevice(), buffers[i], deviceLocalMemory, bufferOffsets[i]);
		}
		for(int i = 0; i < IMAGE_COUNT; i++) {
			vkBindImageMemory(devices->getLogicalDevice(), images[i], deviceLocalMemory, imageOffsets[i]);
		}
	}

	void DeviceLocal::createImageViews() {
		const uint16_t IMAGE_COUNT = createInfo.imageInfos.size();

		imageViews.resize(IMAGE_COUNT, VK_NULL_HANDLE);
		for(int i = 0; i < IMAGE_COUNT; i++) {
			if(createInfo.imageInfos[i].viewInfo != Common::ImageViewInfo{}) {
				imageViews[i] = Common::createImageView(devices->getLogicalDevice(), images[i], createInfo.imageInfos[i].viewInfo);
			}
		}
	}

	void DeviceLocal::createSamplers() {
		if(!createInfo.samplerInfos.empty()) {
			const uint16_t SAMPLER_COUNT = createInfo.samplerInfos.size();

			samplers.resize(SAMPLER_COUNT, VK_NULL_HANDLE);

			for(int i = 0; i < SAMPLER_COUNT; i++) {
				samplers[i] = Common::createSampler(devices->getLogicalDevice(), createInfo.samplerInfos[i]);
			}
		}
	}

	void DeviceLocal::createDescriptorSets() {
		if(!createInfo.descriptorSetInfos.empty()) {
			descriptorPool = Common::createDescriptorPool(devices->getLogicalDevice(), createInfo.descriptorSetInfos);

			descriptorSetLayouts.resize(createInfo.descriptorSetInfos.size(), VK_NULL_HANDLE);
			descriptorSets.resize(createInfo.descriptorSetInfos.size(), VK_NULL_HANDLE);

			for(size_t i = 0; i < createInfo.descriptorSetInfos.size(); i++) {
				createDescriptorSetAndLayout(createInfo.descriptorSetInfos[i], i);
			}
		}
	}

	DeviceLocal::DeviceLocal(Backend::Devices* givenDevices, CreateInfo&& givenCreateInfo, std::function<void(DeviceLocal&)> const& POPULATE_FUNCTION) : 
		devices{ givenDevices },
		deviceLocalMemory{},
		createInfo(std::move(givenCreateInfo)),
		POPULATE(POPULATE_FUNCTION),
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

		createBuffers();
		createImages();
		createMemoryAndBind();
		createImageViews();
		createSamplers();
		createDescriptorSets();

		POPULATE(*this);
	}

	DeviceLocal::~DeviceLocal() {
		vkFreeMemory(devices->getLogicalDevice(), deviceLocalMemory, nullptr);
		for(VkBuffer& buffer : buffers) {
			vkDestroyBuffer(devices->getLogicalDevice(), buffer, nullptr);
		}
		for(VkImage& image : images) {
			vkDestroyImage(devices->getLogicalDevice(), image, nullptr);
		}
		for(VkImageView& imageView : imageViews) {
			vkDestroyImageView(devices->getLogicalDevice(), imageView, nullptr);
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

		Common::createBeginOneTimeCommandBuffer(devices->getLogicalDevice(), tempCommandPool, tempCommandBuffer, devices->getGraphicsQfIndex());
		vkCmdCopyBuffer(tempCommandBuffer, SRC_BUFFER, buffers[INDEX], static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());
		Common::endSubmitDestroyOneTimeCommandBuffer(devices->getLogicalDevice(), devices->getGraphicsQueues()[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::copyBufferToImage(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferImageCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		Common::createBeginOneTimeCommandBuffer(devices->getLogicalDevice(), tempCommandPool, tempCommandBuffer, devices->getGraphicsQfIndex());
		
		// recorded commands
		Common::transitionImageLayout(tempCommandBuffer, images[INDEX],
		VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE, 
		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, devices->getGraphicsQfIndex());

		vkCmdCopyBufferToImage(tempCommandBuffer, SRC_BUFFER, images[INDEX], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());

		Common::transitionImageLayout(tempCommandBuffer, images[INDEX],
		VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, 
		VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, devices->getGraphicsQfIndex());
		// end of recorded commands

		Common::endSubmitDestroyOneTimeCommandBuffer(devices->getLogicalDevice(), devices->getGraphicsQueues()[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::createDescriptorSetAndLayout(Common::DescriptorSetInfo const& INFO, size_t const& INDEX) {
		const VkDescriptorSetLayoutCreateInfo DESCRIPTOR_SET_LAYOUT_INFO{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(INFO.layoutBindings.size()),
			.pBindings = INFO.layoutBindings.data(),
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

		vkUpdateDescriptorSets(devices->getLogicalDevice(), 1, &WRITE_INFO, 0, nullptr);
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

		vkUpdateDescriptorSets(devices->getLogicalDevice(), 1, &WRITE_INFO, 0, nullptr);
	}

	void DeviceLocal::recreateMemory() {
		vkFreeMemory(devices->getLogicalDevice(), deviceLocalMemory, nullptr);
		for(VkBuffer& buffer : buffers) {
			vkDestroyBuffer(devices->getLogicalDevice(), buffer, nullptr);
		}
		for(VkImage& image : images) {
			vkDestroyImage(devices->getLogicalDevice(), image, nullptr);
		}

		createBuffers();
		createImages();
		createMemoryAndBind();
	}

	void DeviceLocal::recreateImageViews() {
		for(VkImageView& imageView : imageViews) {
			vkDestroyImageView(devices->getLogicalDevice(), imageView, nullptr);
		}
		createImageViews();
	}

	void DeviceLocal::recreateDepthResources() {
		const int DEPTH_INDEX = searchForDepthImageIndex();

		if(DEPTH_INDEX != -1) {
			createInfo.imageInfos[DEPTH_INDEX].extent.width = GlobalState::Core::getSwapchain().getCurrentExtent().width;
			createInfo.imageInfos[DEPTH_INDEX].extent.height = GlobalState::Core::getSwapchain().getCurrentExtent().height;
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
