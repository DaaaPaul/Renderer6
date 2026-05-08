#pragma once

#include <vulkan/vulkan.h>
#include <utility>

class Buffer {
	private:
	VkBuffer buffer{};
	void* data{};
	uint32_t size{};

	VkMemoryRequirements memory_requirements{};
	VkBufferUsageFlags usageFlags{};

	public:
	explicit Buffer(void* data, uint32_t size, VkBufferUsageFlags usage);
	~Buffer();

	VkBuffer getBuffer() const { return buffer; }
	VkMemoryRequirements get_memory_requirements() const { return memory_requirements; }
	VkBufferUsageFlags getUsageFlags() const { return usageFlags; }
	void* getData() const { return data; }
	uint32_t getSize() const { return size; } 

	static void copy(Buffer& dst, const Buffer& src);

	private:
	static VkBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage);

	VkBufferCopy getFullRegion() const { return VkBufferCopy{0, 0, this->size}; }
};
