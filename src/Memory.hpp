#pragma once

#include <vulkan/vulkan.h>
#include "Texture.hpp"
#include "DepthImage.hpp"
#include "Buffer.hpp"
#include "DescriptorSet.hpp"

class Memory {
	public:
	struct Measurements {
		VkDeviceSize size{};
		std::vector<VkDeviceSize> texture_offsets{};
		std::vector<VkDeviceSize> buffer_offsets{};
	};

	private:
	VkDeviceMemory memory{};
	Measurements measurements{};
	uint32_t type_index{};

	std::vector<Texture*> p_textures{};
	std::vector<DepthImage*> p_depth_images{};
	std::vector<Buffer*> p_buffers{};

	std::vector<VkDeviceAddress> buffer_addresses{};
	std::vector<void*> buffer_maps{};

	static constexpr VkMemoryAllocateFlagsInfo memory_address_bit{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
		.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
	};

	public:
	Memory() = default;
	explicit Memory(std::vector<Texture>& textures, const std::vector<DepthImage>& depth_images, std::vector<Buffer>& buffers, std::vector<DescriptorSet>& descriptor_sets, VkMemoryPropertyFlags memory_property_flags);
	void destroy() noexcept;

	private:
	void bind_textures(const std::vector<VkDeviceSize>& texture_offsets, const std::vector<Texture>& textures);
	void bind_buffers(const std::vector<VkDeviceSize>& buffer_offsets, const std::vector<Buffer>& buffers);

	static std::vector<Texture*> to_pointers(std::vector<Texture>& textures);
	static std::vector<Buffer*> to_pointers(std::vector<Buffer>& buffers);

	static Measurements calculate_measurements(const std::vector<Texture>& textures, const std::vector<DepthImage>& depth_images, const std::vector<Buffer>& buffers);	
	static VkDeviceSize align_pow_2(const VkDeviceSize& num, const VkDeviceSize& alignment) { return (num + alignment - 1) & ~(alignment - 1); }

	static uint32_t get_memory_type_index(uint32_t index_mask, VkMemoryPropertyFlags property_mask);
	static uint32_t get_memory_index_mask(const std::vector<VkMemoryRequirements>& requirements) {
		uint32_t mask = UINT32_MAX;

		for (const VkMemoryRequirements& requirement : requirements) {
			mask &= requirement.memoryTypeBits;
		}

		return mask;
	}
	template<class... Ts>
	static std::vector<VkMemoryRequirements> get_memory_requirements(const std::vector<Ts>&... memory_object_vectors) {
		std::vector<VkMemoryRequirements> memory_requirements;
		memory_requirements.reserve((0 + ... + memory_object_vectors.size()));

		([&memory_requirements](const auto& memory_object_vector) {
			for(const auto& memory_object : memory_object_vector) {
				memory_requirements.push_back(memory_object.get_memory_requirements());
			}
		}(memory_object_vectors), ...);

		return memory_requirements;
	}

	static std::vector<VkDeviceAddress> address_buffers(const std::vector<Buffer>& buffers);
	static std::vector<void*> map_buffers(VkDeviceMemory memory, const std::vector<VkDeviceSize>& buffer_offsetse, const std::vector<Buffer>& buffers);
	static void fill_buffers(const std::vector<void*>& buffer_maps, std::vector<Buffer>& buffers);
};