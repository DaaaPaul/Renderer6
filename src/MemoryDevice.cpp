#include "MemoryDevice.h"
#include "Resources.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include "MemoryHost.h"
#include "ImageViewHotspot.h"

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
				CHECK_VK_SUCCESS(vkCreateBuffer(gpDevice, &gBufferCreates[i], nullptr, &gBuffers[i].buffer), "Failed to create buffer")
			}
		}

		void populateBufferMemoryRequirements() noexcept {
			gBufferMemoryRequirements.resize(gBuffers.size(), {});

			for(int i = 0; i < gBuffers.size(); i++) {
				vkGetBufferMemoryRequirements(gpDevice, gBuffers[i].buffer, &gBufferMemoryRequirements[i]);
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
				CHECK_VK_SUCCESS(vkCreateImage(gpDevice, &gImageCreates[i], nullptr, &gImages[i].image), "Failed to create image")
			}
		}

		void populateImageMemoryRequirements() noexcept {
			gImageMemoryRequirements.resize(gImages.size(), {});

			for(int i = 0; i < gImages.size(); i++) {
				vkGetImageMemoryRequirements(gpDevice, gImages[i].image, &gImageMemoryRequirements[i]);
				gMemoryItemTypes.push_back((gImageCreates[i].tiling == VK_IMAGE_TILING_OPTIMAL) ? Util::Memory::ItemType::NON_LINEAR : Util::Memory::ItemType::LINEAR);
			}
		}

		void createMemory() {
			gAllMemoryRequirements.insert(gAllMemoryRequirements.end(), gBufferMemoryRequirements.begin(), gBufferMemoryRequirements.end());
			gAllMemoryRequirements.insert(gAllMemoryRequirements.end(), gImageMemoryRequirements.begin(), gImageMemoryRequirements.end());
			gMemoryOffsets = Util::Memory::doMemoryCalculations(gAllMemoryRequirements, gMemoryItemTypes, Backend::PhysicalDevice::gLimits.bufferImageGranularity).second;

			VkDeviceSize memorySize = Util::Memory::doMemoryCalculations(gAllMemoryRequirements, gMemoryItemTypes, Backend::PhysicalDevice::gLimits.bufferImageGranularity).first;
			uint32_t memoryType = Util::Memory::getMemoryTypeIndex(gAllMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

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
			vkAllocateMemory(gpDevice, &memoryAllocate, nullptr, &gpMemory);
		}

		void bindBuffers() {
			for(int i = 0; i < gBuffers.size(); i++) {
				gBuffers[i].offset = gMemoryOffsets[i];
				vkBindBufferMemory(gpDevice, gBuffers[i].buffer, gpMemory, gBuffers[i].offset);
			}
		}

		void populateBufferAddresses() noexcept {
			VkBufferDeviceAddressInfo rollingBufferAddressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
			for(int i = 0; i < gBuffers.size(); i++) {
				if(gBufferCreates[i].usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
					rollingBufferAddressInfo.buffer = gBuffers[i].buffer;
					gBuffers[i].address = vkGetBufferDeviceAddress(gpDevice, &rollingBufferAddressInfo);
				}
			}
		}

		void bindImages() {
			for(int i = gBuffers.size(); i < gMemoryOffsets.size(); i++) {
				gImages[i - gBuffers.size()].offset = gMemoryOffsets[i];
				vkBindImageMemory(gpDevice, gImages[i].image, gpMemory, gImages[i].offset);
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
				CHECK_VK_SUCCESS(vkCreateSampler(gpDevice, &gSamplerCreates[i], nullptr, &gSamplers[i]), "Failed to create sampler")
			}
		}

		void populateDescriptorSetLayoutCreates() noexcept {
			std::vector<VkDescriptorSetLayoutBinding> textureBindings{
				VkDescriptorSetLayoutBinding{
					.binding = 0,
					.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
				},
				VkDescriptorSetLayoutBinding{
					.binding = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
				}
			};
			gDescriptorSetLayoutCreates.push_back(
				VkDescriptorSetLayoutCreateInfo{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
					.bindingCount = UINT32(textureBindings.size()),
					.pBindings = textureBindings.data()
				}
			);
		}

		void createDescriptorSetLayouts() noexcept {
			gDescriptorSets.resize(gDescriptorSetLayoutCreates.size(), {});
			for(int i = 0; i < gDescriptorSetLayoutCreates.size(); i++) {
				CHECK_VK_SUCCESS(vkCreateDescriptorSetLayout(gpDevice, &gDescriptorSetLayoutCreates[i], nullptr, &gDescriptorSets[i].layout), "Failed to create descriptor set");
			}
		}

		void populateDescriptorPoolCreate() noexcept {
			std::vector<VkDescriptorPoolSize> poolSizes{};
			for(VkDescriptorSetLayoutCreateInfo const& LAYOUT_CREATE : gDescriptorSetLayoutCreates) {
				for(int i = 0; i < LAYOUT_CREATE.bindingCount; i++) {
					poolSizes.push_back(
						VkDescriptorPoolSize{
							.type = LAYOUT_CREATE.pBindings[i].descriptorType,
							.descriptorCount = LAYOUT_CREATE.pBindings[i].descriptorCount
						}
					);
				}
			}

			gDescriptorPoolCreate = VkDescriptorPoolCreateInfo{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.maxSets = UINT32(gDescriptorSetLayoutCreates.size()),
				.poolSizeCount = UINT32(poolSizes.size()),
				.pPoolSizes = poolSizes.data()
			};
		}

		void createDescriptorPool() {
			CHECK_VK_SUCCESS(vkCreateDescriptorPool(gpDevice, &gDescriptorPoolCreate, nullptr, &gpDescriptorPool), "Failed to create descriptor pool");
		}

		void populateDescriptorSetAllocates() noexcept {
			gDescriptorSetAllocates.resize(gDescriptorSets.size(), {});

			for(int i = 0; i < gDescriptorSets.size(); i++) {
				gDescriptorSetAllocates[i] = VkDescriptorSetAllocateInfo{
					.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
					.descriptorPool = gpDescriptorPool,
					.descriptorSetCount = 1,
					.pSetLayouts = &gDescriptorSets[i].layout
				};
			}
		}

		void createDescriptorSets() {
			for(int i = 0; i < gDescriptorSets.size(); i++) {
				CHECK_VK_SUCCESS(vkAllocateDescriptorSets(gpDevice, &gDescriptorSetAllocates[i], &gDescriptorSets[i].set), "Failed to create descriptor set");
			}
		}

		void writeToDescriptorSets() {
			Mutate::bindSampledImage(0, 0, 0);
			Mutate::bindSampler(0, 1, 0);
		}

		void deInitMemoryResources() noexcept {
			vkFreeMemory(gpDevice, gpMemory, nullptr);

			for(Util::Memory::BufferBundle& bufferBundle : gBuffers) {
				vkDestroyBuffer(gpDevice, bufferBundle.buffer, nullptr);
			}
			for(Util::Memory::ImageBundle& imageBundle : gImages) {
				vkDestroyImage(gpDevice, imageBundle.image, nullptr);
			}
		}

		void destroySamplers() noexcept {
			for(VkSampler& sampler : gSamplers) {
				vkDestroySampler(gpDevice, sampler, nullptr);
			}
		}

		void deInitDescriptorResources() noexcept {
			for(Util::Memory::DescriptorSetBundle& dsetBundle : gDescriptorSets) {
				vkFreeDescriptorSets(gpDevice, gpDescriptorPool, 1, &dsetBundle.set);
				vkDestroyDescriptorSetLayout(gpDevice, dsetBundle.layout, nullptr);
			}
			vkDestroyDescriptorPool(gpDevice, gpDescriptorPool, nullptr);
		}

		namespace Mutate {
			void copyToBuffer(uint32_t const& INDEX_OF_BUFFER, VkBuffer src, std::vector<VkBufferCopy> const& REGIONS) {
				VkCommandPool tempPool{};
				VkCommandBuffer tempCommandBuffer{};

				Util::Vulkan::beginOneTimeCommandBuffer(tempPool, tempCommandBuffer, Backend::PhysicalDevice::gQueueFamilyIndices[0]);
				vkCmdCopyBuffer(tempCommandBuffer, src, gBuffers[INDEX_OF_BUFFER].buffer, UINT32(REGIONS.size()), REGIONS.data());
				Util::Vulkan::endOneTimeCommandBuffer(Backend::LogicalDevice::gQueues[0], tempPool, tempCommandBuffer);
			}

			void copyToImage(uint32_t const& INDEX_OF_IMAGE, VkBuffer src, std::vector<VkBufferImageCopy> const& REGIONS) {
				VkCommandPool tempPool{};
				VkCommandBuffer tempCommandBuffer{};

				Util::Vulkan::beginOneTimeCommandBuffer(tempPool, tempCommandBuffer, Backend::PhysicalDevice::gQueueFamilyIndices[0]);
		
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

				Util::Vulkan::endOneTimeCommandBuffer(Backend::LogicalDevice::gQueues[0], tempPool, tempCommandBuffer);
			}

			void bindSampledImage(uint32_t const& SET_INDEX, uint32_t const& BINDING, uint32_t const& IMAGE_INDEX) {
				VkDescriptorImageInfo imageInfo{
					.imageView = ImageViewHotspot::newView(VkImageViewCreateInfo{
						.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
						.image = gImages[IMAGE_INDEX].image,
						.viewType = IMAGE_VIEW_TYPE(gImageCreates[IMAGE_INDEX].imageType),
						.format = gImageCreates[IMAGE_INDEX].format,
						.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
					}),
					.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				};

				VkWriteDescriptorSet write{
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.dstSet = gDescriptorSets[SET_INDEX].set,
					.dstBinding = BINDING,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
					.pImageInfo = &imageInfo
				};

				vkUpdateDescriptorSets(gpDevice, 1, &write, 0, nullptr);
			}

			void bindSampler(uint32_t const& SET_INDEX, uint32_t const& BINDING, uint32_t const& SAMPLER_INDEX) {
				VkDescriptorImageInfo samplerInfo{
					.sampler = gSamplers[SAMPLER_INDEX]
				};

				VkWriteDescriptorSet write{
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.dstSet = gDescriptorSets[SET_INDEX].set,
					.dstBinding = BINDING,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
					.pImageInfo = &samplerInfo
				};

				vkUpdateDescriptorSets(gpDevice, 1, &write, 0, nullptr);
			}
		}
	}
}
