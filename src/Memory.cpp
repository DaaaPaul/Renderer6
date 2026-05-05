#include <vulkan/vulkan.h>
#include <cstdlib>
#include "Memory.hpp"
#include "PhysicalDevice.h"

namespace Resource {
	Memory::Memory(std::vector<Texture>& textures, std::vector<Buffer>& buffers, std::vector<DescriptorSet>& descriptorSets) :
		measurements{ getMeasurements(textures, buffers) }, typeIndex{ getType(getMask(getRequirements(textures, buffers)), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) }, pTextures(toPointers(textures)), pBuffers(toPointers(buffers)) {
		memory = createMemory(measurements.size, typeIndex);
		bindTextures(measurements.textureOffsets, textures);
		bindBuffers(measurements.bufferOffsets, buffers);
		
		bufferAddresses = getBufferAddresses(buffers);
		migrateToBuffers(buffers);

		for(Texture& text : textures) {
			text.copyToImage();
		}

		for(DescriptorSet& set : descriptorSets) {
			set.bind();
		}
	}

	Memory::Memory(std::vector<Buffer>& buffers) :
		measurements{ getMeasurements({}, buffers) }, typeIndex{ getType(getMask(getRequirements({}, buffers)), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) }, pTextures{}, pBuffers(toPointers(buffers)) {
		memory = createMemory(measurements.size, typeIndex);
		bindBuffers(measurements.bufferOffsets, buffers);

		bufferAddresses = getBufferAddresses(buffers);
		completeBuffers(measurements.bufferOffsets, buffers);
	}

	Memory::~Memory() {
		vkFreeMemory(gDevice, memory, nullptr);
	}

	std::vector<Texture*> Memory::toPointers(std::vector<Texture>& pTextures) {
		std::vector<Texture*> pointers{};

		for(Texture& text : pTextures) {
			pointers.push_back(&text);
		}

		return pointers;
	}

	std::vector<Buffer*> Memory::toPointers(std::vector<Buffer>& buffers) {
		std::vector<Buffer*> pointers{};

		for(Buffer& buf : buffers) {
			pointers.push_back(&buf);
		}

		return pointers;
	}
	
	VkDeviceMemory Memory::createMemory(VkDeviceSize size, uint32_t typeIndex) {
		VkDeviceMemory memory{};

		VkMemoryAllocateFlagsInfo address{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
			.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
		};
		VkMemoryAllocateInfo memoryAllocate{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = &address,
			.allocationSize = size,
			.memoryTypeIndex = typeIndex
		};
		VK_CHECK(vkAllocateMemory(gDevice, &memoryAllocate, nullptr, &memory), "createMemory: failed")
		
		return memory;
	}

	Memory::Measurements Memory::getMeasurements(std::vector<Texture> const& TEXTURES, std::vector<Buffer> const& BUFFERS) {
		Measurements measurements{};
	
		VkDeviceSize running = 0;

		for(Texture const& TEXT : TEXTURES) {
			running = alignNext(running, TEXT.getRequirements().alignment);
			measurements.textureOffsets.push_back(running);
			running += TEXT.getRequirements().size;
		}

		alignNext(running, PhysicalDevice::gLimits.bufferImageGranularity);

		for(Buffer const& BUF : BUFFERS) {
			running = alignNext(running, BUF.getRequirements().alignment);
			measurements.bufferOffsets.push_back(running);
			running += BUF.getRequirements().size;
		}

		measurements.size = running;

		return measurements;
	}

	uint32_t Memory::getType(uint32_t mask, VkMemoryPropertyFlags propertyMask) {
		VkPhysicalDeviceMemoryProperties memoryProperties{};
		vkGetPhysicalDeviceMemoryProperties(PhysicalDevice::gPhysicalDevice, &memoryProperties);

		uint32_t index = UINT32_MAX;
		for (int i = 0; i < memoryProperties.memoryTypeCount && index == UINT32_MAX; ++i) {
			VkMemoryPropertyFlags propertyFlags = memoryProperties.memoryTypes[i].propertyFlags;

			if((mask & 1 << i) && ((propertyFlags & propertyMask) == propertyMask)) {
				index = i;
			}
		};

		return index;
	}

	uint32_t Memory::getMask(std::vector<VkMemoryRequirements> const& REQS) {
		uint32_t mask = 0b11111111111111111111111111111111;

		for (VkMemoryRequirements const& R : REQS) {
			mask &= R.memoryTypeBits;
		}

		return mask;
	}

	std::vector<VkMemoryRequirements> Memory::getRequirements(std::vector<Texture> const& TEXTURES, std::vector<Buffer> const& BUFFERS) {
		std::vector<VkMemoryRequirements> requirements{};

		for(Texture const& TEXT : TEXTURES) {
			requirements.push_back(TEXT.getRequirements());
		}
		for(Buffer const& BUF : BUFFERS) {
			requirements.push_back(BUF.getRequirements());
		}

		return requirements;
	}

	void Memory::bindTextures(std::vector<VkDeviceSize> const& OFFSETS, std::vector<Texture> const& TEXTURES) {
		assert(OFFSETS.size() == TEXTURES.size());
		
		for(int i = 0; i < TEXTURES.size(); i++) {
			vkBindImageMemory(gDevice, TEXTURES[i].getImage(), memory, OFFSETS[i]);
		}
	}

	void Memory::bindBuffers(std::vector<VkDeviceSize> const& OFFSETS, std::vector<Buffer> const& BUFFERS) {
		assert(OFFSETS.size() == BUFFERS.size());
		
		for(int i = 0; i < BUFFERS.size(); i++) {
			vkBindBufferMemory(gDevice, BUFFERS[i].getBuffer(), memory, OFFSETS[i]);
		}
	}

	std::vector<VkDeviceAddress> Memory::getBufferAddresses(std::vector<Buffer> const& BUFFERS) {
		std::vector<VkDeviceAddress> addresses(BUFFERS.size(), UINT64_MAX);
		
		for(int i = 0; i < BUFFERS.size(); i++) {
			if(BUFFERS[i].getUsageFlags() & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
				addresses[i] = getBufferAddress(BUFFERS[i]);
			}
		}

		return addresses;
	}

	VkDeviceAddress Memory::getBufferAddress(Buffer const& BUFFER) {
		VkBufferDeviceAddressInfo buffer{
			.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
			.buffer = BUFFER.getBuffer()
		};

		return vkGetBufferDeviceAddress(gDevice, &buffer);
	}

	void Memory::completeBuffers(std::vector<VkDeviceAddress> const& OFFSETS, std::vector<Buffer>& buffers) {
		assert(OFFSETS.size() == buffers.size());

		for(int i = 0; i < buffers.size(); i++) {
			copyToBuffer(OFFSETS[i], buffers[i]);
		}
	}

	void Memory::copyToBuffer(VkDeviceAddress offset, Buffer& buffer) {
		std::memcpy(reinterpret_cast<void*>(offset), buffer.getData(), buffer.getSize());
	}

	void Memory::migrateToBuffers(std::vector<Buffer>& buffers) {
		Memory hostVisible(buffers);
		std::vector<Buffer*> hostBuffers = hostVisible.pBuffers;

		for(int i = 0; i < buffers.size(); i++) {
			Buffer::copy(buffers[i], *hostBuffers[i]);
		}
	}
}