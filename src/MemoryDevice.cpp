#include "MemoryDevice.h"
#include "OldResources.h"
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

		void populateBufferCreates() {
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
				CHECK_VK_SUCCESS(vkCreateBuffer(gDevice, &gBufferCreates[i], nullptr, &gBuffers[i].buffer), "Failed to create buffer")
			}
		}

		void populateBufferMemoryRequirements() {
			gBufferMemoryRequirements.resize(gBuffers.size(), {});

			for(int i = 0; i < gBuffers.size(); i++) {
				vkGetBufferMemoryRequirements(gDevice, gBuffers[i].buffer, &gBufferMemoryRequirements[i]);
				gMemoryItemTypes.push_back(Util::Memory::ItemType::LINEAR);
			}
		}

		void populateImageCreates() {
			gImageCreates.push_back(
				VkImageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
					.imageType = VK_IMAGE_TYPE_2D,
					.format = static_cast<VkFormat>(Resources::gTexture->vkFormat),
					.extent = VkExtent3D(Resources::gTexture->baseWidth, Resources::gTexture->baseHeight, 1),
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
					.extent = VkExtent3D(Engine::Swapchain::gStatus.imageExtent.width, Engine::Swapchain::gStatus.imageExtent.height, 1),
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
				CHECK_VK_SUCCESS(vkCreateImage(gDevice, &gImageCreates[i], nullptr, &gImages[i].image), "Failed to create image")
			}
		}

		void populateImageMemoryRequirements() {
			gImageMemoryRequirements.resize(gImages.size(), {});

			for(int i = 0; i < gImages.size(); i++) {
				vkGetImageMemoryRequirements(gDevice, gImages[i].image, &gImageMemoryRequirements[i]);
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
			vkAllocateMemory(gDevice, &memoryAllocate, nullptr, &gMemory);
		}

		void bindBuffers() {
			for(int i = 0; i < gBuffers.size(); i++) {
				gBuffers[i].offset = gMemoryOffsets[i];
				vkBindBufferMemory(gDevice, gBuffers[i].buffer, gMemory, gBuffers[i].offset);
			}
		}

		void populateBufferAddresses() {
			VkBufferDeviceAddressInfo rollingBufferAddressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
			for(int i = 0; i < gBuffers.size(); i++) {
				if(gBufferCreates[i].usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
					rollingBufferAddressInfo.buffer = gBuffers[i].buffer;
					gBuffers[i].address = vkGetBufferDeviceAddress(gDevice, &rollingBufferAddressInfo);
				}
			}
		}

		void bindImages() {
			for(int i = gBuffers.size(); i < gMemoryOffsets.size(); i++) {
				int imageIndex = i - gBuffers.size();

				gImages[imageIndex].offset = gMemoryOffsets[i];
				vkBindImageMemory(gDevice, gImages[imageIndex].image, gMemory, gImages[imageIndex].offset);
			}
		}

		void initializeBufferData() {
			Mutate::copyToBuffer(0, Host::gBuffers[0].buffer, {VkBufferCopy(0, 0, Resources::gModelVertexBufferSize)});
			Mutate::copyToBuffer(1, Host::gBuffers[1].buffer, {VkBufferCopy(0, 0, Resources::gModelIndexBufferSize)});

			for(int i = 0; i < Engine::Swapchain::gIMAGE_COUNT; i++) {
				Mutate::copyToBuffer(2 + i, Host::gBuffers[3 + Engine::Swapchain::gIMAGE_COUNT + i].buffer, {VkBufferCopy(0, 0, Resources::gPARTICLES_BUFFER_SIZE)});
			}
		}

		void initializeImageData() {
			Mutate::copyToImage(0, Host::gBuffers[2].buffer, {VkBufferImageCopy(0, 0, 0, VkImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1), VkOffset3D(0, 0, 0), VkExtent3D(Resources::gTexture->baseWidth, Resources::gTexture->baseHeight, 1))});
		}

		void populateSamplerCreates() {
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
				CHECK_VK_SUCCESS(vkCreateSampler(gDevice, &gSamplerCreates[i], nullptr, &gSamplers[i]), "Failed to create sampler")
			}
		}

		void populateDescriptorSetLayoutCreates() {
			gDescriptorSetLayoutCreateBindings.push_back(
				std::vector<VkDescriptorSetLayoutBinding>{
					VkDescriptorSetLayoutBinding{
						.binding = 0,
						.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
						.descriptorCount = 1,
						.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
					},
					VkDescriptorSetLayoutBinding{
						.binding = 1,
						.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
						.descriptorCount = 1,
						.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
					}
				}
			);

			for(std::vector<VkDescriptorSetLayoutBinding> const& BINDINGS : gDescriptorSetLayoutCreateBindings) {
				gDescriptorSetLayoutCreates.push_back(
					VkDescriptorSetLayoutCreateInfo{
						.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
						.bindingCount = UINT32(BINDINGS.size()),
						.pBindings = BINDINGS.data()
					}
				);
			}
		}

		void createDescriptorSetLayouts() {
			gDescriptorSets.resize(gDescriptorSetLayoutCreates.size(), {});
			for(int i = 0; i < gDescriptorSetLayoutCreates.size(); i++) {
				CHECK_VK_SUCCESS(vkCreateDescriptorSetLayout(gDevice, &gDescriptorSetLayoutCreates[i], nullptr, &gDescriptorSets[i].layout), "Failed to create descriptor set");
			}
		}

		void populateDescriptorPoolCreate() {
			for(VkDescriptorSetLayoutCreateInfo const& LAYOUT : gDescriptorSetLayoutCreates) {
				for(int i = 0; i < LAYOUT.bindingCount; i++) {
					gDescriptorPoolSizes.push_back(
						VkDescriptorPoolSize{
							.type = LAYOUT.pBindings[i].descriptorType,
							.descriptorCount = LAYOUT.pBindings[i].descriptorCount
						}
					);
				}
			}

			gDescriptorPoolCreate = VkDescriptorPoolCreateInfo{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
				.maxSets = UINT32(gDescriptorSetLayoutCreates.size()),
				.poolSizeCount = UINT32(gDescriptorPoolSizes.size()),
				.pPoolSizes = gDescriptorPoolSizes.data()
			};
		}

		void createDescriptorPool() {
			CHECK_VK_SUCCESS(vkCreateDescriptorPool(gDevice, &gDescriptorPoolCreate, nullptr, &gDescriptorPool), "Failed to create descriptor pool");
		}

		void populateDescriptorSetAllocates() {
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
				CHECK_VK_SUCCESS(vkAllocateDescriptorSets(gDevice, &gDescriptorSetAllocates[i], &gDescriptorSets[i].set), "Failed to create descriptor set");
			}
		}

		void writeToDescriptorSets() {
			Mutate::bindSampledImage(0, 0, 0);
			Mutate::bindSampler(0, 1, 0);
		}

		void deInitMemoryResources() {
			vkFreeMemory(gDevice, gMemory, nullptr);

			for(Util::Memory::BufferBundle& bufferBundle : gBuffers) {
				vkDestroyBuffer(gDevice, bufferBundle.buffer, nullptr);
			}
			for(Util::Memory::ImageBundle& imageBundle : gImages) {
				vkDestroyImage(gDevice, imageBundle.image, nullptr);
			}
		}

		void destroySamplers() {
			for(VkSampler sampler : gSamplers) {
				vkDestroySampler(gDevice, sampler, nullptr);
			}
		}

		void deInitDescriptorResources() {
			for(Util::Memory::DescriptorSetBundle& descriptorSetBundle : gDescriptorSets) {
				vkFreeDescriptorSets(gDevice, gDescriptorPool, 1, &descriptorSetBundle.set);
				vkDestroyDescriptorSetLayout(gDevice, descriptorSetBundle.layout, nullptr);
			}
			vkDestroyDescriptorPool(gDevice, gDescriptorPool, nullptr);
		}

		namespace Mutate {
			void copyToBuffer(uint32_t const& INDEX_OF_BUFFER, VkBuffer source, std::vector<VkBufferCopy> const& REGIONS) {
				VkCommandPool tempCmdPool{};
				VkCommandBuffer tempCmdBuffer{};

				Util::Vulkan::beginOneTimeCommandBuffer(tempCmdPool, tempCmdBuffer, Backend::PhysicalDevice::gQueueFamilyIndices[0]);
				vkCmdCopyBuffer(tempCmdBuffer, source, gBuffers[INDEX_OF_BUFFER].buffer, UINT32(REGIONS.size()), REGIONS.data());
				Util::Vulkan::endOneTimeCommandBuffer(Backend::LogicalDevice::gQueues[0], tempCmdPool, tempCmdBuffer);
			}

			void copyToImage(uint32_t const& INDEX_OF_IMAGE, VkBuffer source, std::vector<VkBufferImageCopy> const& REGIONS) {
				VkCommandPool tempCmdPool{};
				VkCommandBuffer tempCommandBuffer{};

				Util::Vulkan::beginOneTimeCommandBuffer(tempCmdPool, tempCommandBuffer, Backend::PhysicalDevice::gQueueFamilyIndices[0]);
		
				Util::Vulkan::transitionImageLayout(tempCommandBuffer, gImages[INDEX_OF_IMAGE].image,
				VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1),
				VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
				VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, Backend::PhysicalDevice::gQueueFamilyIndices[0]);

				vkCmdCopyBufferToImage(tempCommandBuffer, source, gImages[INDEX_OF_IMAGE].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, UINT32(REGIONS.size()), REGIONS.data());
		
				Util::Vulkan::transitionImageLayout(tempCommandBuffer, gImages[INDEX_OF_IMAGE].image,
				VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1),
				VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, 
				VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Backend::PhysicalDevice::gQueueFamilyIndices[0]);

				Util::Vulkan::endOneTimeCommandBuffer(Backend::LogicalDevice::gQueues[0], tempCmdPool, tempCommandBuffer);
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

				vkUpdateDescriptorSets(gDevice, 1, &write, 0, nullptr);
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

				vkUpdateDescriptorSets(gDevice, 1, &write, 0, nullptr);
			}
		}
	}
}
