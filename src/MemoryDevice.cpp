#include "MemoryDevice.h"
#include "Resources.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"

namespace Memory {
	namespace Device {
		void init() {
			initMemoryResources();

			populateSamplerCreates();
			createSamplers();

			initDescriptorResources();
		}

		void deInit() {
			deInitMemoryResources();
			// DE INIT SAMPLERS GO HERE!
			deInitDescriptorResources();
		}


		void initMemoryResources() {
			populateBufferCreates();
			createBuffers();
			populateBufferMemoryRequirements();
			populateImageCreates();
			createImages();
			populateImageMemoryRequirements();
			createMemory();
			bindBuffers();
			populateBufferAddresses();
			bindImages();
			initializeBufferData();
			initializeImageData();
		}

		void initDescriptorResources() {
			populateDescriptorPoolCreate();
			populateDescriptorSetLayoutCreates();
			populateDescriptorSetAllocates();
			createDescriptorSets();
		}


		void populateBufferCreates() noexcept {
			gBufferCreates.push_back(
				VkBufferCreateInfo{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = Resources::gModelVertexBufferSize,
					.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 1,
					.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0]
				}
			);
			gBufferCreates.push_back(
				VkBufferCreateInfo{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = Resources::gModelIndexBufferSize,
					.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 1,
					.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0]
				}
			);
			for(int i = 0; i < Backend::Swapchain::gIMAGE_COUNT; i++) {
				gBufferCreates.push_back(
					VkBufferCreateInfo{
						.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						.size = Resources::gPARTICLES_BUFFER_SIZE,
						.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
						.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
						.queueFamilyIndexCount = 1,
						.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0]
					}
				);
			}
		}

		void createBuffers() {
			gBuffers.resize(gBufferCreates.size(), {});

			for(int i = 0; i < gBufferCreates.size(); i++) {
				CHECK_VK_SUCCESS(vkCreateBuffer(Backend::LogicalDevice::gpDevice, &gBufferCreates[i], nullptr, &gBuffers[i].buffer), "Failed to create buffer")
			}
		}

		void populateBufferMemoryRequirements() noexcept {
			gBufferMemoryRequirements.resize(gBuffers.size(), {});

			for(int i = 0; i < gBuffers.size(); i++) {
				vkGetBufferMemoryRequirements(Backend::LogicalDevice::gpDevice, gBuffers[i].buffer, &gBufferMemoryRequirements[i]);
			}
		}

		void populateImageCreates() noexcept {
			gImageCreates.push_back(
				VkImageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
					.imageType = VK_IMAGE_TYPE_2D,
					.format = static_cast<VkFormat>(Resources::gpTexture->vkFormat),
					.extent = VkExtent3D(Resources::gpTexture->baseWidth, Resources::gpTexture->baseHeight, 1),
					.mipLevels = 1,
					.arrayLayers = 1,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.tiling = VK_IMAGE_TILING_OPTIMAL,
					.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 1,
					.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0],
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				}
			);
			gImageCreates.push_back(
				VkImageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
					.imageType = VK_IMAGE_TYPE_2D,
					.format = VK_FORMAT_D32_SFLOAT,
					.extent = VkExtent3D(Backend::Swapchain::gCurrentSwapchainStatus.imageExtent.width, Backend::Swapchain::gCurrentSwapchainStatus.imageExtent.height, 1),
					.mipLevels = 1,
					.arrayLayers = 1,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.tiling = VK_IMAGE_TILING_OPTIMAL,
					.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 1,
					.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0],
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				}
			);
		}

		void createImages() {
			gImages.resize(gImageCreates.size(), {});

			for(int i = 0; i < gImages.size(); i++) {
				CHECK_VK_SUCCESS(vkCreateImage(Backend::LogicalDevice::gpDevice, &gImageCreates[i], nullptr, &gImages[i].image), "Failed to create image")
			}
		}

		void populateImageMemoryRequirements() noexcept {
			gImageMemoryRequirements.resize(gImages.size(), {});

			for(int i = 0; i < gImages.size(); i++) {
				vkGetImageMemoryRequirements(Backend::LogicalDevice::gpDevice, gImages[i].image, &gImageMemoryRequirements[i]);
			}
		}

		void createMemory() {
			std::vector<VkMemoryRequirements> allMemoryRequirements(gBufferMemoryRequirements);
			allMemoryRequirements.insert(allMemoryRequirements.end(), gImageMemoryRequirements.begin(), gImageMemoryRequirements.end());

			VkDeviceSize memorySize = Util::Memory::calculateMemorySize(allMemoryRequirements);
			uint32_t memoryType = Util::Memory::getMemoryTypeIndex(Backend::PhysicalDevice::gpPhysicalDevice, allMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VkMemoryAllocateFlagsInfo deviceAddressBit{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
				.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
			};
			VkMemoryAllocateInfo memoryAllocate{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.pNext = &deviceAddressBit,
				.allocationSize = memorySize,
				.memoryTypeIndex = memoryType
			};
			vkAllocateMemory(Backend::LogicalDevice::gpDevice, &memoryAllocate, nullptr, &gpMemory);
		}

		void bindBuffers() {
			std::vector<VkDeviceSize> bufferOffsets(Util::Memory::calculateMemoryOffsets(gBufferMemoryRequirements));

			for(int i = 0; i < bufferOffsets.size(); i++) {
				gBuffers[i].offset = bufferOffsets[i];
				vkBindBufferMemory(Backend::LogicalDevice::gpDevice, gBuffers[i].buffer, gpMemory, gBuffers[i].offset);
			}
		}

		void populateBufferAddresses() noexcept {
			VkBufferDeviceAddressInfo rollingBufferAddressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
			for(int i = 0; i < gBuffers.size(); i++) {
				if(gBufferCreates[i].usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
					rollingBufferAddressInfo.buffer = gBuffers[i].buffer;
					gBuffers[i].address = vkGetBufferDeviceAddress(Backend::LogicalDevice::gpDevice, &rollingBufferAddressInfo);
				}
			}
		}

		void bindImages() {

		}

		void initializeBufferData() noexcept {

		}

		void initializeImageData() noexcept {

		}


		void populateSamplerCreates() noexcept {

		}

		void createSamplers() {

		}


		void populateDescriptorPoolCreate() noexcept {

		}

		void populateDescriptorSetLayoutCreates() noexcept {

		}

		void populateDescriptorSetAllocates() noexcept {

		}

		void createDescriptorSets() {
		
		}

		void deInitMemoryResources() noexcept {

		}

		void deInitDescriptorResources() noexcept {

		}
	}





































	void DeviceLocal::setupMemory() {

		// bind buffers and images
		bufferOffsets.assign(memoryOffsets.begin(), memoryOffsets.begin() + BUFFER_COUNT);
		imageOffsets.assign(memoryOffsets.begin() + BUFFER_COUNT, memoryOffsets.end());

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
				std::cout << "DEVICE LOCAL MEMORY: ";
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
			const uint16_t DESCRIPTOR_SET_COUNT = createInfo.descriptorSetInfos.size();

			pDescriptorPool = createDescriptorPool(pDevices->getLogicalDevice(), createInfo.descriptorSetInfos);

			descriptorSetLayouts.resize(DESCRIPTOR_SET_COUNT, VK_NULL_HANDLE);
			descriptorSets.resize(DESCRIPTOR_SET_COUNT, VK_NULL_HANDLE);

			for(int i = 0; i < DESCRIPTOR_SET_COUNT; i++) {
				descriptorSetLayouts[i] = createDescriptorSetLayout(pDevices->getLogicalDevice(), createInfo.descriptorSetInfos[i]);
				descriptorSets[i] = createDescriptorSet(pDevices->getLogicalDevice(), pDescriptorPool, descriptorSetLayouts[i], createInfo.descriptorSetInfos[i]);
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
		setupMemory();
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

		beginOneTimeCommandBuffer(pDevices->getLogicalDevice(), tempCommandPool, tempCommandBuffer, pDevices->getGraphicsQfIndex());
		vkCmdCopyBuffer(tempCommandBuffer, SRC_BUFFER, buffers[INDEX], static_cast<uint32_t>(COPY_REGIONS.size()), COPY_REGIONS.data());
		endOneTimeCommandBuffer(pDevices->getLogicalDevice(), pDevices->getGraphicsQueues()[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::copyBufferToImage(size_t const& INDEX, VkBuffer const& SRC_BUFFER, std::vector<VkBufferImageCopy> const& COPY_REGIONS) {
		VkCommandPool tempCommandPool{};
		VkCommandBuffer tempCommandBuffer{};

		beginOneTimeCommandBuffer(pDevices->getLogicalDevice(), tempCommandPool, tempCommandBuffer, pDevices->getGraphicsQfIndex());
		
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

		endOneTimeCommandBuffer(pDevices->getLogicalDevice(), pDevices->getGraphicsQueues()[0], tempCommandPool, tempCommandBuffer);
	}

	void DeviceLocal::descriptorSetBindingToBuffers(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& BUFFER_DESCRIPTOR_INDICES) {
		if(BUFFER_DESCRIPTOR_INDICES.size() != createInfo.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorCount) {
			throw std::runtime_error("Number of buffers must match number of descriptors in set " + std::to_string(SET_INDEX) + " binding " + std::to_string(SET_BINDING_NUM));
		}
	
		std::vector<VkDescriptorBufferInfo> toWriteBuffers{};
		for(size_t const& BUFFER_INDEX : BUFFER_DESCRIPTOR_INDICES) {
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

	void DeviceLocal::descriptorSetBindingToCombinedImageSampler(size_t const& SET_INDEX, uint32_t const& SET_BINDING_NUM, std::vector<size_t> const& SAMPLER_IMAGE_DESCRIPTOR_INDICES) {
		if(SAMPLER_IMAGE_DESCRIPTOR_INDICES.size() != createInfo.descriptorSetInfos[SET_INDEX].layoutBindings[SET_BINDING_NUM].descriptorCount) {
			throw std::runtime_error("Number of images/samplers must match number of descriptors in set " + std::to_string(SET_INDEX) + " binding " + std::to_string(SET_BINDING_NUM));
		}

		std::vector<VkDescriptorImageInfo> toWriteImageSamplers{};
		for(size_t const& SAMPLER_IMAGE_INDEX : SAMPLER_IMAGE_DESCRIPTOR_INDICES) {
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
		setupMemory();
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
