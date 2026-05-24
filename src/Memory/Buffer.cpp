#include "Backend/PhysicalDevice.h"
#include "Buffer.hpp"
#include "Utility/Vulkan.h"

Buffer::Buffer(uint32_t name_index,
			   VkBufferCreateFlags create_flags, 
			   VkDeviceSize size, 
			   VkBufferUsageFlags usage_flags, 
			   VkSharingMode sharing_mode,
			   const std::vector<uint32_t>& queue_family_indices) : name_index{ name_index } {
	VkBufferCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = create_flags,
		.size = size,
		.usage = usage_flags,
		.sharingMode = sharing_mode,
		.queueFamilyIndexCount = UINT32(queue_family_indices.size()),
		.pQueueFamilyIndices = queue_family_indices.data()
	};

	VK_CHECK(vkCreateBuffer(g_device, &create, nullptr, &buffer), "Buffer: failed")
}

void Buffer::destroy() noexcept {
	vkDestroyBuffer(g_device, buffer, nullptr);
}

void Buffer::copy_buffer(const Buffer& src, Buffer& dst, VkBufferCopy region) {
	VkCommandPool pool{};
	VkCommandBuffer one_time_cmds{};

	Vulkan::begin_one_time_cmd_buffer(pool, one_time_cmds, PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT));
	vkCmdCopyBuffer(one_time_cmds, src.buffer, dst.buffer, 1, &region);
	Vulkan::end_one_time_cmd_buffer(LogicalDevice::get_queue(VK_QUEUE_GRAPHICS_BIT), pool, one_time_cmds);
}
