#include <vulkan/vulkan.h>
#include <cstdlib>
#include "Memory.hpp"
#include "Backend/PhysicalDevice.h"
#include "Utility/Vulkan.h"

Memory::Memory(const std::vector<Buffer*>& p_buffers, const std::vector<Image*>& p_images, VkMemoryPropertyFlags memory_property_flags, const void* memory_create_p_next) :
	properties{ get_properties(p_buffers, p_images, memory_property_flags) }, p_buffers(p_buffers), p_images(p_images) {
	
	memory = Vulkan::create_memory(properties.size, properties.memory_type_index, memory_create_p_next);
	bind_memory(memory, Buffer::get_vk_buffers(p_buffers), Image::get_vk_images(p_images), properties.buffer_offsets, properties.image_offsets);
}

void Memory::destroy() noexcept {
	vkFreeMemory(g_device, memory, nullptr);
}

void Memory::bind_memory(VkDeviceMemory memory, const std::vector<VkBuffer>& buffers, const std::vector<VkImage>& images, const std::vector<VkDeviceSize>& buffer_offsets, const std::vector<VkDeviceSize>& image_offsets) {
	assert(buffers.size() == buffer_offsets.size());
	assert(images.size() == image_offsets.size());

	for(int i = 0; i < buffers.size(); ++i) {
		vkBindBufferMemory(g_device, buffers[i], memory, buffer_offsets[i]);
	}
	for(int i = 0; i < images.size(); ++i) {
		vkBindImageMemory(g_device, images[i], memory, image_offsets[i]);
	}
}

Memory::Properties Memory::get_properties(const std::vector<Buffer*>& buffers, const std::vector<Image*>& images, VkMemoryPropertyFlags memory_property_flags) {
	Properties properties{};
	VkDeviceSize running = 0;

	VkMemoryRequirements buffer_requirements{};
	for(Buffer* buffer : buffers) {
		buffer_requirements = buffer->get_memory_requirements();

		running = align_pow_2(running, buffer_requirements.alignment);
		properties.buffer_offsets.push_back(running);
		running += buffer_requirements.size;
	}

	running = align_pow_2(running, PhysicalDevice::g_limits.bufferImageGranularity);

	VkMemoryRequirements image_requirements{};
	for(Image* image : images) {
		image_requirements = image->get_memory_requirements();

		running = align_pow_2(running, image_requirements.alignment);
		properties.image_offsets.push_back(running);
		running += image_requirements.size;
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