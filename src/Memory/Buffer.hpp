#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include "Backend/LogicalDevice.h"

struct Buffer {
	VkBuffer buffer{};
	
	explicit Buffer(VkBufferCreateFlags create_flags, 
					VkDeviceSize size,
					VkBufferUsageFlags usage_flags, 
					VkSharingMode sharing_mode,
					const std::vector<uint32_t>& queue_family_indices);
	~Buffer() {
		vkDestroyBuffer(g_device, buffer, nullptr);
	};

	VkMemoryRequirements get_memory_requirements() const;

	static void copy_buffer(const Buffer* src, Buffer* dst, VkBufferCopy region);
	static std::vector<VkBuffer> get_vk_buffers(const std::vector<Buffer*>& p_buffers);
	static std::vector<VkDeviceSize> get_buffer_sizes(const std::vector<Buffer*>& p_buffers);
};