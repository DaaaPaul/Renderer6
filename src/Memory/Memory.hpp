#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "Image.hpp"
#include "Buffer.hpp"

class Memory {
	protected:
	struct Properties {
		VkDeviceSize size{};
		uint32_t memory_type_index{};
		std::vector<VkDeviceSize> image_offsets{};
		std::vector<VkDeviceSize> buffer_offsets{};
	};

	VkDeviceMemory memory{};
	Properties properties{};

	std::vector<Buffer*> p_buffers{};
	std::vector<Image*> p_images{};

	public:
	Memory() = default;

	explicit Memory(const std::vector<Buffer*>& p_buffers, const std::vector<Image*>& p_images, VkMemoryPropertyFlags memory_property_flags, const void* memory_create_p_next);
	virtual void destroy();

	private:
	static void bind_memory(VkDeviceMemory memory, const std::vector<VkBuffer>& buffers, const std::vector<VkImage>& images, const std::vector<VkDeviceSize>& buffer_offsets, const std::vector<VkDeviceSize>& image_offsets);

	static Properties get_properties(const std::vector<Buffer*>& buffers, const std::vector<Image*>& images, VkMemoryPropertyFlags memory_property_flags);	

	static VkDeviceSize align_pow_2(const VkDeviceSize& num, const VkDeviceSize& alignment) { 
		return (num + alignment - 1) & ~(alignment - 1); 
	}

	static std::vector<VkMemoryRequirements> get_memory_requirements(const std::vector<Buffer*>& buffers, const std::vector<Image*>& images) {
		std::vector<VkMemoryRequirements> requirements(buffers.size() + images.size());

		for(int i = 0; i < buffers.size(); ++i) {
			requirements[i] = buffers[i]->get_memory_requirements();
		}

		for(int i = 0; i < images.size(); ++i) {
			requirements[i + buffers.size()] = images[i]->get_memory_requirements();
		}

		return requirements;
	}

	static uint32_t get_memory_index_mask(const std::vector<VkMemoryRequirements>& requirements) {
		uint32_t mask = 0xFFFFFFFF;

		for (const VkMemoryRequirements& requirement : requirements) {
			mask &= requirement.memoryTypeBits;
		}

		return mask;
	}

	static uint32_t get_memory_type_index(uint32_t index_mask, VkMemoryPropertyFlags property_mask);
};