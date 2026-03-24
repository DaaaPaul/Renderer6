#include <utility>
#include <iostream>
#include "MemoryHost.h"
#include "Resources.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include "Transforms.hpp"

namespace Memory {
	namespace Host {
		void init() {
			populateBufferCreates();
			createBuffers();
			populateBufferMemoryRequirements();
			createMemory();
			bindBuffers();
			populateBufferAddresses();
			initializeBufferData();
		}

		void deInit() {
			vkFreeMemory(Backend::LogicalDevice::gpDevice, gpMemory, nullptr);

			for(Util::Memory::BufferBundle& bundle : gBuffers) {
				vkDestroyBuffer(Backend::LogicalDevice::gpDevice, bundle.buffer, nullptr);
			}
		}

		void populateBufferCreates() noexcept {
			gBufferCreates.push_back(
				VkBufferCreateInfo{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = Resources::gModelVertexBufferSize,
					.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 1,
					.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0]
				}
			);
			gBufferCreates.push_back(
				VkBufferCreateInfo{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = Resources::gModelIndexBufferSize,
					.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 1,
					.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0]
				}
			);
			gBufferCreates.push_back(
				VkBufferCreateInfo{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = Resources::gpTexture->dataSize,
					.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 1,
					.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0]
				}
			);

			for(int i = 0; i < Engine::Swapchain::gIMAGE_COUNT; i++) {
				gBufferCreates.push_back(
					VkBufferCreateInfo{
						.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						.size = sizeof(Vertex::Transforms),
						.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
						.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
						.queueFamilyIndexCount = 1,
						.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0]
					}
				);
			}
			for(int i = 0; i < Engine::Swapchain::gIMAGE_COUNT; i++) {
				gBufferCreates.push_back(
					VkBufferCreateInfo{
						.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						.size = Resources::gPARTICLES_BUFFER_SIZE,
						.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
						.queueFamilyIndexCount = 1,
						.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0]
					}
				);
			}
			for(int i = 0; i < Engine::Swapchain::gIMAGE_COUNT; i++) {
				gBufferCreates.push_back(
					VkBufferCreateInfo{
						.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						.size = sizeof(float),
						.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
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

		void createMemory() {
			VkDeviceSize memorySize = Util::Memory::doMemoryCalculations(gBufferMemoryRequirements, gMemoryItemTypes, Backend::PhysicalDevice::gLimits.bufferImageGranularity).first;
			uint32_t memoryType = Util::Memory::getMemoryTypeIndex(Backend::PhysicalDevice::gpPhysicalDevice, gBufferMemoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		
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
			std::vector<VkDeviceSize> bufferOffsets(Util::Memory::doMemoryCalculations(gBufferMemoryRequirements, gMemoryItemTypes, Backend::PhysicalDevice::gLimits.bufferImageGranularity).second);

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

		void initializeBufferData() noexcept {
			Mutate::writeToBuffer(0, Resources::gModelVertices.data(), Resources::gModelVertexBufferSize);
			Mutate::writeToBuffer(1, Resources::gModelVertexIndices.data(), Resources::gModelIndexBufferSize);
			Mutate::writeToBuffer(2, Resources::gpTexture->pData, Resources::gpTexture->dataSize);

			for(int i = 0; i < Engine::Swapchain::gIMAGE_COUNT; i++) {
				Mutate::writeToBuffer(3 + i, &Engine::getCurrentTransformation(), sizeof(Vertex::Transforms));
			}
			for(int i = 0; i < Engine::Swapchain::gIMAGE_COUNT; i++) {
				Mutate::writeToBuffer(7 + i, Resources::gParticles.data(), Resources::gPARTICLES_BUFFER_SIZE);
			}
		}

		namespace Mutate {
			void writeToBuffer(uint32_t const& INDEX_OF_BUFFER, void const* pDATA, uint32_t const& SIZE_TO_WRITE) {
				void* addressOfFirstByte{};
				CHECK_VK_SUCCESS(vkMapMemory(Backend::LogicalDevice::gpDevice, gpMemory, gBuffers[INDEX_OF_BUFFER].offset, gBufferMemoryRequirements[INDEX_OF_BUFFER].size, 0, &addressOfFirstByte), "Failed to map memory")
				std::memcpy(addressOfFirstByte, pDATA, SIZE_TO_WRITE);
				vkUnmapMemory(Backend::LogicalDevice::gpDevice, gpMemory);
			}
		}
	}
}