#include "PhysicalDevice.h"
#include "Buffer.hpp"
#include "Util.h"

Buffer::Buffer(void* data, uint32_t size, VkBufferUsageFlags usageFlags) : 
	data{ data }, size{ size }, usageFlags{ usageFlags } {
	buffer = createBuffer(size, usageFlags);

	vkGetBufferMemoryRequirements(g_device, buffer, &memory_requirements);
}

Buffer::~Buffer() {
	delete data;
	vkDestroyBuffer(g_device, buffer, nullptr);
}

VkBuffer Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage) {
	VkBuffer buffer{};

	VkBufferCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &PhysicalDevice::g_queue_family_indices[0]
	};

	VK_CHECK(vkCreateBuffer(g_device, &create, nullptr, &buffer), "createBuffer: failed")

	return buffer;
}

void Buffer::copy(Buffer& dst, const Buffer& src) {
	VkCommandPool tempPool{};
	VkCommandBuffer tempCmds{};
		
	const VkBufferCopy FULL = src.getFullRegion();

	Util::begin(tempPool, tempCmds, PhysicalDevice::g_queue_family_indices[0]);
	vkCmdCopyBuffer(tempCmds, src.buffer, dst.buffer, 1, &FULL);
	Util::end(LogicalDevice::gQueues[0], tempPool, tempCmds);
}
