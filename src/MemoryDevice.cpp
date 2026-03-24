#include "MemoryDevice.h"
#include "Resources.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include "MemoryHost.h"

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
			destroySamplers();
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
			populateDescriptorSetLayoutCreates();
			populateDescriptorPoolCreate();
			createDescriptorSetLayouts();
			createDescriptorPool();
			populateDescriptorSetAllocates();
			createDescriptorSets();
			writeToDescriptorSets();
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
			for(int i = 0; i < Engine::Swapchain::gIMAGE_COUNT; i++) {
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
				gMemoryItemTypes.push_back(Util::Memory::ItemType::LINEAR);
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
					.extent = VkExtent3D(Engine::Swapchain::gCurrentSwapchainStatus.imageExtent.width, Engine::Swapchain::gCurrentSwapchainStatus.imageExtent.height, 1),
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
				gMemoryItemTypes.push_back((gImageCreates[i].tiling == VK_IMAGE_TILING_OPTIMAL) ? Util::Memory::ItemType::NON_LINEAR : Util::Memory::ItemType::LINEAR);
			}
		}

		void createMemory() {
			gAllMemoryRequirements.insert(gAllMemoryRequirements.end(), gBufferMemoryRequirements.begin(), gBufferMemoryRequirements.end());
			gAllMemoryRequirements.insert(gAllMemoryRequirements.end(), gImageMemoryRequirements.begin(), gImageMemoryRequirements.end());
			gMemoryOffsets = Util::Memory::doMemoryCalculations(gAllMemoryRequirements, gMemoryItemTypes, Backend::PhysicalDevice::gLimits.bufferImageGranularity).second;

			VkDeviceSize memorySize = Util::Memory::doMemoryCalculations(gAllMemoryRequirements, gMemoryItemTypes, Backend::PhysicalDevice::gLimits.bufferImageGranularity).first;
			uint32_t memoryType = Util::Memory::getMemoryTypeIndex(Backend::PhysicalDevice::gpPhysicalDevice, gAllMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

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
			for(int i = 0; i < gBuffers.size(); i++) {
				gBuffers[i].offset = gMemoryOffsets[i];
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
			for(int i = gBuffers.size(); i < gMemoryOffsets.size(); i++) {
				gImages[i - gBuffers.size()].offset = gMemoryOffsets[i];
				vkBindImageMemory(Backend::LogicalDevice::gpDevice, gImages[i].image, gpMemory, gImages[i].offset);
			}
		}

		void initializeBufferData() noexcept {
			Mutate::copyToBuffer(0, Host::gBuffers[0].buffer, {VkBufferCopy(0, 0, Resources::gModelVertexBufferSize)});
			Mutate::copyToBuffer(1, Host::gBuffers[1].buffer, {VkBufferCopy(0, 0, Resources::gModelIndexBufferSize)});

			for(int i = 0; i < Engine::Swapchain::gIMAGE_COUNT; i++) {
				Mutate::copyToBuffer(2 + i, Host::gBuffers[3 + Engine::Swapchain::gIMAGE_COUNT + i].buffer, {VkBufferCopy(0, 0, Resources::gPARTICLES_BUFFER_SIZE)});
			}
		}

		void initializeImageData() noexcept {
			Mutate::copyToImage(2, Host::gBuffers[2].buffer, {VkBufferImageCopy(0, 0, 0, VkImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1), VkOffset3D(0, 0, 0), VkExtent3D(Resources::gpTexture->baseWidth, Resources::gpTexture->baseHeight, 1))});
		}


		void populateSamplerCreates() noexcept {
			gSamplerCreates.push_back(
				VkSamplerCreateInfo{
					.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
					.magFilter = VK_FILTER_LINEAR,
					.minFilter = VK_FILTER_LINEAR,
					.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
					.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
					.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
					.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
					.anisotropyEnable = VK_TRUE,
					.maxAnisotropy = Backend::PhysicalDevice::gLimits.maxSamplerAnisotropy,
					.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
					.unnormalizedCoordinates = VK_FALSE
				}
			);
		}

		void createSamplers() {
			gSamplers.resize(gSamplerCreates.size(), {});

			for(int i = 0; i < gSamplers.size(); i++) {
				CHECK_VK_SUCCESS(vkCreateSampler(Backend::LogicalDevice::gpDevice, &gSamplerCreates[i], nullptr, &gSamplers[i]), "Failed to create sampler")
			}
		}

		void populateDescriptorSetLayoutCreates() noexcept {
			std::vector<VkDescriptorSetLayoutBinding> combinedImageSamplerBinding{
				VkDescriptorSetLayoutBinding{
					.binding = 0,
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
				}
			};
			gDescriptorSetLayoutCreates.push_back(
				VkDescriptorSetLayoutCreateInfo{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
					.bindingCount = UINT32(combinedImageSamplerBinding.size()),
					.pBindings = combinedImageSamplerBinding.data()
				}
			);
		}

		void createDescriptorSetLayouts() noexcept {
			gDescriptorSets.resize(gDescriptorSetLayoutCreates.size(), {});
			for(int i = 0; i < gDescriptorSetLayoutCreates.size(); i++) {
				CHECK_VK_SUCCESS(vkCreateDescriptorSetLayout(Backend::LogicalDevice::gpDevice, &gDescriptorSetLayoutCreates[i], nullptr, &gDescriptorSets[i].layout), "Failed to create descriptor set");
			}
		}

		void populateDescriptorPoolCreate() noexcept {
			std::vector<VkDescriptorPoolSize> poolSizes{};
			for(VkDescriptorSetLayoutCreateInfo const& LAYOUT_CREATE : gDescriptorSetLayoutCreates) {
				// Warning: assumes each descriptor set only has 1 binding
				poolSizes.push_back(
					VkDescriptorPoolSize{
						.type = LAYOUT_CREATE.pBindings[0].descriptorType,
						.descriptorCount = LAYOUT_CREATE.pBindings[0].descriptorCount
					}
				);
			}

			gDescriptorPoolCreate = VkDescriptorPoolCreateInfo{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.maxSets = UINT32(gDescriptorSetLayoutCreates.size()),
				.poolSizeCount = UINT32(poolSizes.size()),
				.pPoolSizes = poolSizes.data()
			};
		}

		void createDescriptorPool() {
			CHECK_VK_SUCCESS(vkCreateDescriptorPool(Backend::LogicalDevice::gpDevice, &gDescriptorPoolCreate, nullptr, &gDescriptorPool), "Failed to create descriptor pool");
		}

		void populateDescriptorSetAllocates() noexcept {
			gDescriptorSetAllocates.resize(gDescriptorSets.size(), {});

			for(int i = 0; i < gDescriptorSets.size(); i++) {
				gDescriptorSetAllocates[i] = VkDescriptorSetAllocateInfo{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
					.descriptorPool = gDescriptorPool,
					.descriptorSetCount = 1,
					.pSetLayouts = &gDescriptorSets[i].layout
				};
			}
		}

		void createDescriptorSets() {
			for(int i = 0; i < gDescriptorSets.size(); i++) {
				CHECK_VK_SUCCESS(vkAllocateDescriptorSets(Backend::LogicalDevice::gpDevice, &gDescriptorSetAllocates[i], &gDescriptorSets[i].set), "Failed to create descriptor set");
			}
		}

		void writeToDescriptorSets() {
			Mutate::bindSampler(0, 0, 0, 0);
		}

		void deInitMemoryResources() noexcept {

		}

		void destroySamplers() noexcept {
		
		}

		void deInitDescriptorResources() noexcept {

		}

		namespace Mutate {
			void copyToBuffer(uint32_t const& INDEX_OF_BUFFER, VkBuffer src, std::vector<VkBufferCopy> const& REGIONS) {
				VkCommandPool tempPool{};
				VkCommandBuffer tempCommandBuffer{};

				Util::Vulkan::beginOneTimeCommandBuffer(Backend::LogicalDevice::gpDevice, tempPool, tempCommandBuffer, Backend::PhysicalDevice::gQueueFamilyIndices[0]);
				vkCmdCopyBuffer(tempCommandBuffer, src, gBuffers[INDEX_OF_BUFFER].buffer, UINT32(REGIONS.size()), REGIONS.data());
				Util::Vulkan::endOneTimeCommandBuffer(Backend::LogicalDevice::gpDevice, Backend::LogicalDevice::gQueues[0], tempPool, tempCommandBuffer);
			}

			void copyToImage(uint32_t const& INDEX_OF_IMAGE, VkBuffer src, std::vector<VkBufferImageCopy> const& REGIONS) {
				VkCommandPool tempPool{};
				VkCommandBuffer tempCommandBuffer{};

				Util::Vulkan::beginOneTimeCommandBuffer(Backend::LogicalDevice::gpDevice, tempPool, tempCommandBuffer, Backend::PhysicalDevice::gQueueFamilyIndices[0]);
		
				Util::Vulkan::transitionImageLayout(tempCommandBuffer, gImages[INDEX_OF_IMAGE].image,
				VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1),
				VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
				VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Backend::PhysicalDevice::gQueueFamilyIndices[0]);

				vkCmdCopyBufferToImage(tempCommandBuffer, src, gImages[INDEX_OF_IMAGE].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, UINT32(REGIONS.size()), REGIONS.data());
		
				Util::Vulkan::transitionImageLayout(tempCommandBuffer, gImages[INDEX_OF_IMAGE].image,
				VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1),
				VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, 
				VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Backend::PhysicalDevice::gQueueFamilyIndices[0]);

				Util::Vulkan::endOneTimeCommandBuffer(Backend::LogicalDevice::gpDevice, Backend::LogicalDevice::gQueues[0], tempPool, tempCommandBuffer);
			}

			void bindSampler(uint32_t const& SET_INDEX, uint32_t const& BINDING, uint32_t const& SAMPLER_INDEX, uint32_t const& IMAGE_INDEX) {
				VkDescriptorImageInfo write{
					.sampler = gSamplers[SAMPLER_INDEX],
					.imageView = gImages[IMAGE_INDEX].view,
					.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				};
				VkWriteDescriptorSet overallWrite{
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.dstSet = gDescriptorSets[SET_INDEX].set,
					.dstBinding = BINDING,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.pImageInfo = &write
				};
				vkUpdateDescriptorSets(Backend::LogicalDevice::gpDevice, 1, &overallWrite, 0, nullptr);
			}
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
