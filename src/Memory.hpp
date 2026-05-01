#pragma once

#include <vulkan/vulkan.h>
#include "Texture.hpp"
#include "Buffer.hpp"
#include "DescriptorSet.hpp"

namespace Resource {
	class Memory {
		private:
		VkDeviceMemory memory{};
		VkDeviceSize size{};
		std::vector<VkDeviceSize> offsets{};
		std::vector<VkDeviceAddress> bufferAddresses{};

		public:
		explicit Memory(std::vector<Texture> const&, std::vector<Buffer> const&, std::vector<DescriptorSet> const&, VkMemoryPropertyFlags);
		~Memory();

		private:
		static std::pair<VkDeviceSize, std::vector<VkDeviceSize>> sizeAndOffsets(std::vector<Texture> const&, std::vector<Buffer> const&);
	};
}