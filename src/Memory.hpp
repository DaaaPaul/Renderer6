#pragma once

#include <vulkan/vulkan.h>

namespace Resource {
	class Memory {
		private:
		VkDeviceMemory memory{};

		public:
		explicit Buffer(VkDeviceSize size, VkBufferUsageFlags usage);
		~Buffer();

		private:
		static VkBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage);
	};
}