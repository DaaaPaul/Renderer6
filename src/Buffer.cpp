#include "PhysicalDevice.h"
#include "Buffer.hpp"
#include "Util.h"

namespace Resource {
	Buffer::Buffer(void* data, uint32_t size, VkBufferUsageFlags usageFlags) : 
		data{ data }, size{ size }, usageFlags{ usageFlags } {
		buffer = createBuffer(size, usageFlags);

		vkGetBufferMemoryRequirements(gDevice, buffer, &requirements);
	}

	Buffer::~Buffer() {
		delete data;
		vkDestroyBuffer(gDevice, buffer, nullptr);
	}

	VkBuffer Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage) {
		VkBuffer buffer{};

		VkBufferCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = size,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &PhysicalDevice::gQueueFamilyIndices[0]
		};

		VK_CHECK(vkCreateBuffer(gDevice, &create, nullptr, &buffer), "createBuffer: failed")

		return buffer;
	}

	void Buffer::copy(Buffer& dst, Buffer const& SRC) {
		VkCommandPool tempPool{};
		VkCommandBuffer tempCmds{};
		
		const VkBufferCopy FULL = SRC.getFullRegion();

		Util::begin(tempPool, tempCmds, PhysicalDevice::gQueueFamilyIndices[0]);
		vkCmdCopyBuffer(tempCmds, SRC.buffer, dst.buffer, 1, &FULL);
		Util::end(LogicalDevice::gQueues[0], tempPool, tempCmds);
	}
}