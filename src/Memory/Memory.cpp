#include <vulkan/vulkan.h>
#include <cstdlib>
#include "Memory.hpp"
#include "Backend/PhysicalDevice.h"

Memory::Memory(std::vector<Buffer>& buffers, std::vector<Image>& images, VkMemoryPropertyFlags memory_property_flags) :
	properties{ get_properties(buffers, images, memory_property_flags) }, p_buffers(Utility::to_pointers(buffers)), p_images(Utility::to_pointers(images)) {
	
	memory = Vulkan::create_memory(properties.size, properties.memory_type_index, &memory_address_bit);
}

void Memory::destroy() noexcept {
	vkFreeMemory(g_device, memory, nullptr);
}

void Memory::bind_memory(VkDeviceMemory memory, std::vector<VkBuffer>& buffers, std::vector<VkImage>& images, const std::vector<VkDeviceSize>& buffer_offsets, const std::vector<VkDeviceSize>& image_offsets) {
	assert(buffers.size() == buffer_offsets.size());
	assert(images.size() == image_offsets.size());

	for(int i = 0; i < buffers.size(); i++) {
		vkBindImageMemory(g_device, images[i], memory, image_offsets[i]);
	}
	for(int i = 0; i < images.size(); i++) {
		vkBindBufferMemory(g_device, buffers[i], memory, buffer_offsets[i]);
	}
}

Memory::Properties Memory::get_properties(const std::vector<Buffer>& buffers, const std::vector<Image>& images, VkMemoryPropertyFlags memory_property_flags) {
	Properties properties{};
	VkDeviceSize running = 0;

	for(const Buffer& buffer : buffers) {
		running = align_pow_2(running, buffer.get_memory_requirements().alignment);
		properties.buffer_offsets.push_back(running);
		running += buffer.get_memory_requirements().size;
	}

	align_pow_2(running, PhysicalDevice::g_limits.bufferImageGranularity);

	for(const Image& image : images) {
		running = align_pow_2(running, image.get_memory_requirements().alignment);
		properties.image_offsets.push_back(running);
		running += image.get_memory_requirements().size;
	}

	properties.size = running;

	properties.memory_type_index = get_memory_type_index(get_memory_index_mask(get_memory_requirements(buffers, images)), memory_property_flags);

	return properties;
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