#include <utility>
#include "Engine.h"
#include "MemoryHost.h"
#include "OldResources.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include "TransformMatrices.hpp"
#include "LogicalDevice.h"

namespace Memory {
	namespace Host {
		void init() {
			populateBufferCreates();
			createBuffers();
			populateBufferMemoryRequirements();
			createMemory();
			bind_buffers();
			populateBufferAddresses();
			initializeBufferData();
		}

		void destroy() {
			vkFreeMemory(g_device, gMemory, nullptr);

			for(Utility::Memory::BufferBundle& bundle : gBuffers) {
				vkDestroyBuffer(g_device, bundle.buffer, nullptr);
			}
		}

		void populateBufferCreates() {
			gBufferCreates.push_back(
				VkBufferCreateInfo{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = Resources::g_vertex_buffer_size,
					.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 1,
					.pQueueFamilyIndices = &PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT)
				}
			);
			gBufferCreates.push_back(
				VkBufferCreateInfo{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = Resources::g_index_buffer_size,
					.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 1,
					.pQueueFamilyIndices = &PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT)
				}
			);
			gBufferCreates.push_back(
				VkBufferCreateInfo{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = Resources::g_texture->dataSize,
					.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
					.queueFamilyIndexCount = 1,
					.pQueueFamilyIndices = &PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT)
				}
			);

			for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
				gBufferCreates.push_back(
					VkBufferCreateInfo{
						.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						.size = sizeof(TransformMatrices),
						.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
						.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
						.queueFamilyIndexCount = 1,
						.pQueueFamilyIndices = &PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT)
					}
				);
			}
		}

		void createBuffers() {
			gBuffers.resize(gBufferCreates.size(), {});

			for(int i = 0; i < gBufferCreates.size(); ++i) {
				VK_CHECK(vkCreateBuffer(g_device, &gBufferCreates[i], nullptr, &gBuffers[i].buffer), "Failed to create buffer")
			}
		}

		void populateBufferMemoryRequirements() {
			gBufferMemoryRequirements.resize(gBuffers.size(), {});

			for(int i = 0; i < gBuffers.size(); ++i) {
				vkGetBufferMemoryRequirements(g_device, gBuffers[i].buffer, &gBufferMemoryRequirements[i]);
				gMemoryItemTypes.push_back(Utility::Memory::ItemType::LINEAR);
			}
		}

		void createMemory() {
			VkDeviceSize memorySize = Utility::Memory::doMemoryCalculations(gBufferMemoryRequirements, gMemoryItemTypes, PhysicalDevice::g_limits.bufferImageGranularity).first;
			uint32_t memoryType = Utility::Memory::getMemoryTypeIndex(gBufferMemoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		
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
			vkAllocateMemory(g_device, &memoryAllocate, nullptr, &gMemory);
		}

		void bind_buffers() {
			std::vector<VkDeviceSize> buffer_offsets(Utility::Memory::doMemoryCalculations(gBufferMemoryRequirements, gMemoryItemTypes, PhysicalDevice::g_limits.bufferImageGranularity).second);

			for(int i = 0; i < buffer_offsets.size(); ++i) {
				gBuffers[i].offset = buffer_offsets[i];
				vkBindBufferMemory(g_device, gBuffers[i].buffer, gMemory, gBuffers[i].offset);
			}
		}

		void populateBufferAddresses() {
			VkBufferDeviceAddressInfo rollingBufferAddressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
			for(int i = 0; i < gBuffers.size(); ++i) {
				if(gBufferCreates[i].usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
					rollingBufferAddressInfo.buffer = gBuffers[i].buffer;
					gBuffers[i].address = vkGetBufferDeviceAddress(g_device, &rollingBufferAddressInfo);
				}
			}
		}

		void initializeBufferData() {
			Mutate::writeToBuffer(0, Resources::g_model_vertices.data(), Resources::g_vertex_buffer_size);
			Mutate::writeToBuffer(1, Resources::g_model_indices.data(), Resources::g_index_buffer_size);
			Mutate::writeToBuffer(2, Resources::g_texture->pData, Resources::g_texture->dataSize);
		}

		namespace Mutate {
			void writeToBuffer(uint32_t const& INDEX_OF_BUFFER, void const* DATA, uint32_t const& SIZE_TO_WRITE) {
				void* bufferPointer{};
				VK_CHECK(vkMapMemory(g_device, gMemory, gBuffers[INDEX_OF_BUFFER].offset, gBufferMemoryRequirements[INDEX_OF_BUFFER].size, 0, &bufferPointer), "Failed to map memory")
				std::memcpy(bufferPointer, DATA, SIZE_TO_WRITE);
				vkUnmapMemory(g_device, gMemory);
			}
		}
	}
}