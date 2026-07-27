#include <vulkan/vulkan.h>
#include <cstdlib>
#include "Memory.hpp"
#include "Backend/PhysicalDevice.h"
#include "Utility/Vulkan.h"

Memory::Memory(const std::vector<Buffer>& buffers, const std::vector<Image>& images, VkMemoryPropertyFlags memory_property_flags, const void* memory_create_p_next) :
	memory_property_flags{ memory_property_flags }, specs{ get_properties(buffers, images, memory_property_flags) }, buffers(buffers), images(images) {
	
	memory = Vulkan::create_memory(specs.size, specs.memory_type_index, memory_create_p_next);
	bind_memory(memory, buffers, images, specs.buffer_offsets, specs.image_offsets);
}

void Memory::destroy() {
	vkFreeMemory(g_device, memory, nullptr);
}

void Memory::copy_to_buffer(const void* p_data, uint32_t size, Buffer buffer) {
	uint32_t buffer_index = Utility::INVALID_UINT32;
	
	for(int i = 0; i < buffers.size() && buffer_index == Utility::INVALID_UINT32; ++i) {
		if(buffers[i].buffer == buffer.buffer) {
			buffer_index = i;
		}
	}

	if(buffer_index == Utility::INVALID_UINT32) {
		throw std::runtime_error("copy_to_buffer: could not find buffer");
	}

	void* p_buffer = nullptr;

	vkMapMemory(g_device, memory, specs.buffer_offsets[buffer_index], size, Vulkan::NO_FLAGS, &p_buffer);
	memcpy(p_buffer, p_data, size);
	vkUnmapMemory(g_device, memory);
}

void Memory::bind_memory(VkDeviceMemory memory, const std::vector<Buffer>& buffers, const std::vector<Image>& images, const std::vector<VkDeviceSize>& buffer_offsets, const std::vector<VkDeviceSize>& image_offsets) {
	assert(buffers.size() == buffer_offsets.size());
	assert(images.size() == image_offsets.size());

	for(int i = 0; i < buffers.size(); ++i) {
		vkBindBufferMemory(g_device, buffers[i].buffer, memory, buffer_offsets[i]);
	}
	for(int i = 0; i < images.size(); ++i) {
		vkBindImageMemory(g_device, images[i].image, memory, image_offsets[i]);
	}
}

Memory::Specs Memory::get_properties(const std::vector<Buffer>& buffers, const std::vector<Image>& images, VkMemoryPropertyFlags memory_property_flags) {
	Specs specs{};
	VkDeviceSize running = 0;

	std::vector<VkMemoryRequirements> memory_requirements(get_memory_requirements(buffers, images));

	for(int i = 0; i < buffers.size(); ++i) {
		running = align_pow_2(running, memory_requirements[i].alignment);
		specs.buffer_offsets.push_back(running);
		running += memory_requirements[i].size;
	}

	running = align_pow_2(running, PhysicalDevice::g_limits.bufferImageGranularity);

	for(int i = 0; i < images.size(); ++i) {
		running = align_pow_2(running, memory_requirements[i + buffers.size()].alignment);
		specs.image_offsets.push_back(running);
		running += memory_requirements[i + buffers.size()].size;
	}

	specs.size = running;

	specs.memory_type_index = get_memory_type_index(get_memory_index_mask(memory_requirements), memory_property_flags);

	return specs;
}

std::vector<VkMemoryRequirements> Memory::get_memory_requirements(const std::vector<Buffer>& buffers, const std::vector<Image>& images) {
	std::vector<VkMemoryRequirements> requirements(buffers.size() + images.size());

	for(int i = 0; i < buffers.size(); ++i) {
		requirements[i] = buffers[i].get_memory_requirements();
	}

	for(int i = 0; i < images.size(); ++i) {
		requirements[i + buffers.size()] = images[i].get_memory_requirements();
	}

	return requirements;
}

uint32_t Memory::get_memory_index_mask(const std::vector<VkMemoryRequirements>& requirements) {
	uint32_t mask = 0xFFFFFFFF;

	for (const VkMemoryRequirements& requirement : requirements) {
		mask &= requirement.memoryTypeBits;
	}

	return mask;
}

uint32_t Memory::get_memory_type_index(uint32_t index_mask, VkMemoryPropertyFlags property_mask) {
	VkPhysicalDeviceMemoryProperties memory_information{};
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice::g_physical_device, &memory_information);

	uint32_t memory_index = Utility::INVALID_UINT32;

	for (int i = 0; i < memory_information.memoryTypeCount && memory_index == Utility::INVALID_UINT32; ++i) {
		if ((index_mask & (1 << i)) && ((memory_information.memoryTypes[i].propertyFlags & property_mask) == property_mask)) {
			memory_index = i;
		}
	};

	if(memory_index == Utility::INVALID_UINT32) {
		throw std::runtime_error("get_memory_type_index: did not find index");
	}

	return memory_index;
}