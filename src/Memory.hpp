#pragma once

#include <vulkan/vulkan.h>
#include "Texture.hpp"
#include "Buffer.hpp"
#include "DescriptorSet.hpp"

namespace Resource {
	class Memory {
		private:
		struct Measurements {
			VkDeviceSize size{};
			std::vector<VkDeviceSize> textureOffsets{};
			std::vector<VkDeviceSize> bufferOffsets{};
		};

		VkDeviceMemory memory{};
		Measurements measurements{};
		uint32_t typeIndex{};

		std::vector<Texture*> pTextures{};
		std::vector<Buffer*> pBuffers{};

		std::vector<VkDeviceAddress> bufferAddresses{};

		public:
		explicit Memory(std::vector<Texture>&, std::vector<Buffer>&, std::vector<DescriptorSet> const&);
		~Memory();

		private:
		explicit Memory(std::vector<Buffer>&);

		static std::vector<Texture*> toPointers(std::vector<Texture>&);
		static std::vector<Buffer*> toPointers(std::vector<Buffer>&);

		static VkDeviceMemory createMemory(VkDeviceSize size, uint32_t typeIndex);
		static Measurements getMeasurements(std::vector<Texture> const&, std::vector<Buffer> const&);
		static uint32_t getType(uint32_t, VkMemoryPropertyFlags);
		static uint32_t getMask(std::vector<VkMemoryRequirements> const&);
		static std::vector<VkMemoryRequirements> getRequirements(std::vector<Texture> const&, std::vector<Buffer> const&);
		static VkDeviceSize alignNext(VkDeviceSize const& N, VkDeviceSize const& ALIGNMENT) { return (N + ALIGNMENT - 1) & ~(ALIGNMENT - 1); }

		static VkDeviceMemory bindTextures(VkDeviceMemory, std::vector<VkDeviceSize> const&, std::vector<Texture> const&);
		static VkDeviceMemory bindBuffers(VkDeviceMemory, std::vector<VkDeviceSize> const&, std::vector<Buffer> const&);

		static std::vector<VkDeviceAddress> getBufferAddresses(std::vector<Buffer> const&);

		static void completeBuffers(std::vector<VkDeviceAddress> const&, std::vector<Buffer>&);
		static void migrateToBuffers(std::vector<Buffer>&);

		static void completeTextures(std::vector<Texture>&);
		static void copyToImage(ktxTexture2* texture, VkImage image);
	};
}