#pragma once

#include <vulkan/vulkan_core.h>
#include <vma/vk_mem_alloc.h>
#include <vector>
#include <cstdint>
#include "backend/LogicalDevice.h"

class Buffer {
	public:
	Buffer() = default;
	explicit Buffer(VkBufferCreateFlags create_flags, 
					VkDeviceSize size,
					VkBufferUsageFlags usage_flags, 
					VkSharingMode sharing_mode,
					const std::vector<uint32_t>& queue_family_indices,
					VmaAllocationCreateInfo vma_allocation_info);
	void destroy() {
		vkDestroyBuffer(g_device, buffer, nullptr);
	};

	VkBuffer get_buffer() const {
		return buffer;
	}
	VkMemoryRequirements get_memory_requirements() const {
		VkMemoryRequirements memory_requirements{};
		vkGetBufferMemoryRequirements(g_device, buffer, &memory_requirements);

		return memory_requirements; 
	}
	VmaAllocationInfo get_allocation_info() const {
		return allocation_info;
	}

	static void copy_to(const void* p_data, Buffer* dst, size_t size);
	static void copy(const Buffer* src, Buffer* dst, VkBufferCopy region);
	static std::vector<VkDeviceSize> get_buffer_sizes(const std::vector<Buffer*>& buffers);

	private:
	VkBuffer buffer{};
	VmaAllocation allocation{};
	VmaAllocationInfo allocation_info{};
};