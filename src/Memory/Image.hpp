#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include "backend/LogicalDevice.h"
#include "utility/Utility.h"

struct Image {
	VkImage image{};

	explicit Image(VkImageCreateFlags create_flags, 
				   VkImageType image_type,    
				   VkFormat format, 
				   VkExtent3D extent, 
				   uint32_t mip_level_count,
				   uint32_t array_layer_count,
				   VkSampleCountFlagBits sample_count,
				   VkImageUsageFlags usage,
				   VkSharingMode sharing_mode, 
				   const std::vector<uint32_t>& queue_family_indices);
	void destroy() {
		vkDestroyImage(g_device, image, nullptr);
	}

	VkMemoryRequirements get_memory_requirements() const;

	static void transition_image_layout(VkImage image, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mip_level_count, uint32_t array_layer_count);
	static void copy_to_image(VkImage image, const void* p_data, VkOffset3D offset, VkExtent3D extent, uint32_t mip_level);
};
