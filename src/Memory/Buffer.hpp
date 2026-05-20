#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class Buffer {
	private:
	VkBuffer buffer{};
	
	public:
	virtual ~Buffer() = default;
	explicit Buffer(VkBufferCreateFlags create_flags, 
					VkDeviceSize size,
					VkBufferUsageFlags usage_flags, 
					VkSharingMode sharing_mode,
					const std::vector<uint32_t>& queue_family_indices);
	virtual void destroy() noexcept;

	VkBuffer get_buffer() const { 
		return buffer; 
	}

	VkMemoryRequirements get_memory_requirements() const { 
		VkMemoryRequirements memory_requirements{};
		vkGetBufferMemoryRequirements(g_device, buffer, &memory_requirements);

		return memory_requirements; 
	}

	static void copy_buffer(const Buffer& src, Buffer& dst, VkBufferCopy region);
};