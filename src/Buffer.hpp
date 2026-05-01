#pragma once

#include <vulkan/vulkan.h>

namespace Resource {
	class Buffer {
		private:
		VkBuffer buffer{};

		VkMemoryRequirements requirements{};

		public:
		explicit Buffer(VkDeviceSize size, VkBufferUsageFlags usage);
		~Buffer();

		VkMemoryRequirements getRequirements() const { return requirements; }

		private:
		static VkBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage);
	};
}