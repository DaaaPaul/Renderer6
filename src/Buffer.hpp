#pragma once

#include <vulkan/vulkan.h>
#include <utility>

class Buffer {
	private:
	VkBuffer buffer{};
	void* data{};
	uint32_t size{};

	VkMemoryRequirements memory_requirements{};
	VkBufferUsageFlags usage_flags{};

	public:
	explicit Buffer(void* data, uint32_t size, VkBufferUsageFlags usage);
	~Buffer();

	VkBuffer get_buffer() const { return buffer; }
	VkMemoryRequirements get_memory_requirements() const { return memory_requirements; }
	VkBufferUsageFlags get_usage_flags() const { return usage_flags; }
	void* get_datra() const { return data; }
	uint32_t get_size() const { return size; } 

	static void copy(Buffer& dst, const Buffer& src);

	private:
	VkBufferCopy get_full_region() const { return VkBufferCopy{0, 0, this->size}; }
};