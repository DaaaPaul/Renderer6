#pragma once

#include <vulkan/vulkan.h>
#include "Texture.hpp"
#include "DepthImage.hpp"
#include "Buffer.hpp"
#include "DescriptorSet.hpp"

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
	std::vector<DepthImage*> pDepthImages{};
	std::vector<Buffer*> pBuffers{};

	std::vector<VkDeviceAddress> bufferAddresses{};
	std::vector<void*> bufferMaps{};

	public:
	Memory() = default;
	explicit Memory(std::vector<Texture>&, std::vector<DepthImage> const&, std::vector<Buffer>&, std::vector<DescriptorSet>&);
	explicit Memory(std::vector<Buffer>&);
	~Memory();

	private:
	static std::vector<Texture*> toPointers(std::vector<Texture>&);
	static std::vector<Buffer*> toPointers(std::vector<Buffer>&);

	static VkDeviceMemory createMemory(VkDeviceSize size, uint32_t typeIndex);
	static Measurements getMeasurements(std::vector<Texture> const&, std::vector<DepthImage> const&, std::vector<Buffer> const&);
	static VkDeviceSize alignNext(VkDeviceSize const& N, VkDeviceSize const& ALIGNMENT) { return (N + ALIGNMENT - 1) & ~(ALIGNMENT - 1); }

	static uint32_t getType(uint32_t, VkMemoryPropertyFlags);
	static uint32_t getMask(std::vector<VkMemoryRequirements> const&);
	static std::vector<VkMemoryRequirements> getRequirements(std::vector<Texture> const&, std::vector<DepthImage> const&, std::vector<Buffer> const&);

	void bindTextures(std::vector<VkDeviceSize> const&, std::vector<Texture> const&);
	void bindBuffers(std::vector<VkDeviceSize> const&, std::vector<Buffer> const&);

	static std::vector<VkDeviceAddress> getBufferAddresses(std::vector<Buffer> const&);
	static VkDeviceAddress getBufferAddress(Buffer const&);

	std::vector<void*> mapBuffers(std::vector<VkDeviceSize> const&, std::vector<Buffer> const&);
	void* mapBuffer(VkDeviceSize const&, Buffer const&);

	static void completeBuffers(std::vector<void*> const&, std::vector<Buffer>&);
	static void copyToBuffer(void*, Buffer&);
	static void migrateToBuffers(std::vector<Buffer>&);
};
