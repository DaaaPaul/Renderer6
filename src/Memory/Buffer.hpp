#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include "Backend/LogicalDevice.h"

class Buffer {
	private:
	VkBuffer buffer{};
	
	public:
	explicit Buffer(VkBufferCreateFlags create_flags, 
					VkDeviceSize size,
					VkBufferUsageFlags usage_flags, 
					VkSharingMode sharing_mode,
					const std::vector<uint32_t>& queue_family_indices);
	virtual ~Buffer() {
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

	static void copy_buffer(const Buffer* src, Buffer* dst, VkBufferCopy region);

	static std::vector<VkBuffer> get_vk_buffers(const std::vector<Buffer*>& p_buffers) {
		std::vector<VkBuffer> vk_buffers(p_buffers.size());

		for(int i = 0; i < p_buffers.size(); ++i) {
			vk_buffers[i] = p_buffers[i]->get_buffer();
		}

		return vk_buffers;
	}

	static std::vector<VkDeviceSize> get_buffer_sizes(const std::vector<Buffer*>& p_buffers) {
		std::vector<VkDeviceSize> buffer_sizes(p_buffers.size());

		for(int i = 0; i < p_buffers.size(); ++i) {
			buffer_sizes[i] = p_buffers[i]->get_memory_requirements().size;
		}

		return buffer_sizes;
	}
};