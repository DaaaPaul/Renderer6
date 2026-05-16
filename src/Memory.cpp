#include <vulkan/vulkan.h>
#include <cstdlib>
#include "Memory.hpp"
#include "PhysicalDevice.h"

Memory::Memory(std::vector<Texture>& textures, const std::vector<DepthImage>& depth_images, std::vector<Buffer>& buffers, std::vector<DescriptorSet>& descriptor_sets, VkMemoryPropertyFlags memory_property_flags) :
	measurements{ calculate_measurements(textures, depth_images, buffers) }, type_index{ get_memory_type_index(get_memory_index_mask(get_memory_requirements(textures, depth_images, buffers)), memory_property_flags) }, p_textures(to_pointers(textures)), p_buffers(to_pointers(buffers)) {
	
	memory = Vulkan::create_memory(measurements.size, type_index, &memory_address_bit);
	bind_textures(measurements.texture_offsets, textures);
	bind_buffers(measurements.buffer_offsets, buffers);

	buffer_addresses = address_buffers(buffers);

	for(Texture& text : textures) {
		text.copyToImage();
	}

	for(DescriptorSet& set : descriptor_sets) {
		set.bind();
	}
}

Memory::~Memory() {
	//vkFreeMemory(g_device, memory, nullptr);
}

void Memory::bind_textures(const std::vector<VkDeviceSize>& OFFSETS, const std::vector<Texture>& TEXTURES) {
	assert(OFFSETS.size() == TEXTURES.size());
		
	for(int i = 0; i < TEXTURES.size(); ++i) {
		vkBindImageMemory(g_device, TEXTURES[i].get_image(), memory, OFFSETS[i]);
	}
}

void Memory::bind_buffers(const std::vector<VkDeviceSize>& OFFSETS, const std::vector<Buffer>& BUFFERS) {
	assert(OFFSETS.size() == BUFFERS.size());
		
	for(int i = 0; i < BUFFERS.size(); ++i) {
		vkBindBufferMemory(g_device, BUFFERS[i].get_buffer(), memory, OFFSETS[i]);
	}
}

std::vector<Texture*> Memory::to_pointers(std::vector<Texture>& p_textures) {
	std::vector<Texture*> pointers{};

	for(Texture& text : p_textures) {
		pointers.push_back(&text);
	}

	return pointers;
}

std::vector<Buffer*> Memory::to_pointers(std::vector<Buffer>& buffers) {
	std::vector<Buffer*> pointers{};

	for(Buffer& buf : buffers) {
		pointers.push_back(&buf);
	}

	return pointers;
}

Memory::Measurements Memory::calculate_measurements(const std::vector<Texture>& textures, const std::vector<DepthImage>& depth_images, const std::vector<Buffer>& buffers) {
	Measurements measurements{};
	VkDeviceSize running = 0;

	for(const Texture& texture : textures) {
		running = align_pow_2(running, texture.get_memory_requirements().alignment);
		measurements.texture_offsets.push_back(running);
		running += texture.get_memory_requirements().size;
	}

	for(const DepthImage& depth_image : depth_images) {
		running = align_pow_2(running, depth_image.get_memory_requirements().alignment);
		running += depth_image.get_memory_requirements().alignment;
	}

	align_pow_2(running, PhysicalDevice::g_limits.bufferImageGranularity);

	for(const Buffer& buffer : buffers) {
		running = align_pow_2(running, buffer.get_memory_requirements().alignment);
		measurements.buffer_offsets.push_back(running);
		running += buffer.get_memory_requirements().size;
	}

	measurements.size = running;

	return measurements;
}

uint32_t Memory::get_memory_type_index(uint32_t index_mask, VkMemoryPropertyFlags property_mask) {
	VkPhysicalDeviceMemoryProperties memory_information{};
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice::g_physical_device, &memory_information);

	uint32_t memory_index = UINT32_MAX;
	for (int i = 0; i < memory_information.memoryTypeCount && memory_index == UINT32_MAX; ++i) {
		if ((index_mask & (1 << i)) && ((memory_information.memoryTypes[i].propertyFlags & property_mask) == property_mask)) {
			memory_index = i;
		}
	};

	return memory_index;
}

std::vector<VkDeviceAddress> Memory::address_buffers(const std::vector<Buffer>& buffers) {
	std::vector<VkDeviceAddress> addresses(buffers.size(), UINT64_MAX);
		
	for(int i = 0; i < buffers.size(); ++i) {
		if(buffers[i].getUsageFlags() & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
			VkBufferDeviceAddressInfo buffer_address{
				.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
				.buffer = buffers[i].get_buffer()
			};
			
			addresses[i] = vkGetBufferDeviceAddress(g_device, &buffer_address);
		}
	}

	return addresses;
}

std::vector<void*> Memory::map_buffers(VkDeviceMemory memory, const std::vector<VkDeviceSize>& buffer_offsets, const std::vector<Buffer>& buffers) {
	std::vector<void*> maps{};
		
	for(int i = 0; i < buffers.size(); ++i) {
		void* map{};
		VK_CHECK(vkMapMemory(g_device, memory, buffer_offsets[i], buffers[i].getSize(), VK_NO_FLAGS, &map), "map_buffers: failed")
		maps.push_back(map);
	}

	return maps;
}

void Memory::fill_buffers(const std::vector<void*>& buffer_maps, std::vector<Buffer>& buffers) {
	for(int i = 0; i < buffers.size(); ++i) {
		std::memcpy(buffer_maps[i], buffers[i].getData(), buffers[i].getSize());
	}
}