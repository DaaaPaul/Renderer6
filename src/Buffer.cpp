#include "PhysicalDevice.h"
#include "Buffer.hpp"
#include "Util.h"

namespace Resource {
	Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage) {
		buffer = createBuffer(size, usage);

		vkGetBufferMemoryRequirements(gDevice, buffer, &requirements);
	}

	Buffer::~Buffer() {
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
			.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0]
		};

		VK_CHECK(vkCreateBuffer(gDevice, &create, nullptr, &buffer), "createBuffer: failed")

		return buffer;
	}
}