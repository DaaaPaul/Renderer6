#pragma once

#include <vulkan/vulkan_core.h>
#include <vma/vk_mem_alloc.h>
#include <vector>
#include <cstdint>
#include "backend/LogicalDevice.h"
#include "utility/Vulkan.h"

class Image {
	public:
	Image() = default;
	explicit Image(VkImageCreateFlags create_flags, 
				   VkImageType image_type,    
				   VkFormat format, 
				   VkExtent3D extent, 
				   uint32_t mip_level_count,
				   uint32_t array_layer_count,
				   VkSampleCountFlagBits sample_count,
				   VkImageUsageFlags usage,
				   VkSharingMode sharing_mode, 
				   const std::vector<uint32_t>& queue_family_indices,
				   VmaAllocationCreateInfo vma_allocation_info);
	void destroy() {
		vmaDestroyImage(Vulkan::g_vma_allocator, image, allocation);
	}

	VkImage get_image() const {
		return image;
	}
	VkMemoryRequirements get_memory_requirements() const {
		VkMemoryRequirements memory_requirements{};
		vkGetImageMemoryRequirements(g_device, image, &memory_requirements);
	
		return memory_requirements;
	}
	VmaAllocationInfo get_allocation_info() const {
		return allocation_info;
	}

	static void transition_layout(Image image, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mip_level_count, uint32_t array_layer_count);
	static void copy_to(Image image, const void* p_data, VkOffset3D offset, VkExtent3D extent, uint32_t mip_level);

	private:
	VkImage image{};
	VmaAllocation allocation{};
	VmaAllocationInfo allocation_info{};
};