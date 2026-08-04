#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include "backend/LogicalDevice.h"
#include "backend/PhysicalDevice.h"
#include "Buffer.hpp"
#include "utility/Utility.h"
#include "utility/Vulkan.h"

Buffer::Buffer(VkBufferCreateFlags create_flags, 
			   VkDeviceSize size, 
			   VkBufferUsageFlags usage_flags, 
			   VkSharingMode sharing_mode,
			   const std::vector<uint32_t>& queue_family_indices,
			   VmaAllocationCreateInfo vma_allocation_info) {
	VkBufferCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = create_flags,
		.size = size,
		.usage = usage_flags,
		.sharingMode = sharing_mode,
		.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size()),
		.pQueueFamilyIndices = queue_family_indices.data()
	};

	Vulkan::check(vmaCreateBuffer(Vulkan::g_vma_allocator, &create, &vma_allocation_info, &buffer, &allocation, &allocation_info), "Buffer: failed");
}

void Buffer::copy_to(const void* p_data, Buffer* dst, size_t size) {
	void* p_buffer = dst->get_allocation_info().pMappedData;
	
	if(!p_buffer) {
		throw std::runtime_error("copy_to: !p_buffer");
	}

	memcpy(p_buffer, p_data, size);
}

void Buffer::copy(const Buffer* src, Buffer* dst, VkBufferCopy region) {
	VkCommandPool pool{};
	VkCommandBuffer one_time_cmds{};

	Vulkan::begin_one_time_cmd_buffer(pool, one_time_cmds, PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT));
	vkCmdCopyBuffer(one_time_cmds, src->buffer, dst->buffer, 1, &region);
	Vulkan::end_one_time_cmd_buffer(LogicalDevice::get_queue(VK_QUEUE_GRAPHICS_BIT), pool, one_time_cmds);
}

std::vector<VkDeviceSize> Buffer::get_buffer_sizes(const std::vector<Buffer*>& buffers) {
	std::vector<VkDeviceSize> buffer_sizes(buffers.size());

	for(int i = 0; i < buffers.size(); ++i) {
		buffer_sizes[i] = buffers[i]->get_memory_requirements().size;
	}

	return buffer_sizes;
}