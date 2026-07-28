#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "Image.hpp"
#include "Buffer.hpp"

class Memory {
	public:
	Memory() = default;

	explicit Memory(const std::vector<Buffer>& buffers, const std::vector<Image>& images, VkMemoryPropertyFlags memory_property_flags, const void* memory_create_p_next);
	void destroy();

	void copy_to_buffer(const void* p_data, Buffer buffer, uint32_t offset, uint32_t size);

	private:
	struct Specs {
		VkDeviceSize size{};
		uint32_t memory_type_index{};
		std::vector<VkDeviceSize> image_offsets{};
		std::vector<VkDeviceSize> buffer_offsets{};
	};

	VkDeviceMemory memory{};
	VkMemoryPropertyFlags memory_property_flags{};
	Specs specs{};

	std::vector<Buffer> buffers{};
	std::vector<Image> images{};

	static VkDeviceSize align_pow_2(const VkDeviceSize& num, const VkDeviceSize& alignment) { 
		return (num + alignment - 1) & ~(alignment - 1); 
	}

	static void bind_memory(VkDeviceMemory memory, const std::vector<Buffer>& buffers, const std::vector<Image>& images, const std::vector<VkDeviceSize>& buffer_offsets, const std::vector<VkDeviceSize>& image_offsets);
	static Specs get_properties(const std::vector<Buffer>& buffers, const std::vector<Image>& images, VkMemoryPropertyFlags memory_property_flags);	
	static std::vector<VkMemoryRequirements> get_memory_requirements(const std::vector<Buffer>& buffers, const std::vector<Image>& images);
	static uint32_t get_memory_index_mask(const std::vector<VkMemoryRequirements>& requirements);
	static uint32_t get_memory_type_index(uint32_t index_mask, VkMemoryPropertyFlags property_mask);
};