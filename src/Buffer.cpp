#include "PhysicalDevice.h"
#include "Buffer.hpp"
#include "Vulkan.h"

Buffer::Buffer(void* data, uint32_t size, VkBufferUsageFlags usage_flags) : 
	buffer{ Vulkan::create_buffer(size, usage_flags) }, data{ data }, size{ size }, usage_flags{ usage_flags } {

	vkGetBufferMemoryRequirements(g_device, buffer, &memory_requirements);
}

void Buffer::destroy() noexcept {
	delete data;
	vkDestroyBuffer(g_device, buffer, nullptr);
}

void Buffer::copy(Buffer& dst, const Buffer& src) {
	VkCommandPool pool{};
	VkCommandBuffer one_time_cmds{};
	const VkBufferCopy FULL_SRC_REGION = src.get_full_region();

	Vulkan::begin_one_time_cmd_buffer(pool, one_time_cmds, PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT));
	vkCmdCopyBuffer(one_time_cmds, src.buffer, dst.buffer, 1, &FULL_SRC_REGION);
	Vulkan::end_one_time_cmd_buffer(LogicalDevice::get_queue(VK_QUEUE_GRAPHICS_BIT), pool, one_time_cmds);
}
