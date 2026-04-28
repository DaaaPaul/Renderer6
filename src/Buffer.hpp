#pragma once

#include <vulkan/vulkan.h>
#include "Util.h"

namespace Engine {
	class Buffer {
		private:
		VkBuffer buffer{};

		VkMemoryRequirements requirements{};

		public:
		explicit Buffer(VkDeviceSize size, VkBufferUsageFlags usage);
		~Buffer();

		private:
		static VkBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage);
	};
}