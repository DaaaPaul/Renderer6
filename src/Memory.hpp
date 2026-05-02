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
		static void copyToImage(ktxTexture2* texture, VkImage image);
		static std::pair<VkDeviceSize, std::vector<VkDeviceSize>> sizeAndOffsets(std::vector<Texture> const&, std::vector<Buffer> const&);
		static VkDeviceSize alignNext(VkDeviceSize const& N, VkDeviceSize const& ALIGNMENT) { return (N + ALIGNMENT - 1) & ~(ALIGNMENT - 1); }
	};
}