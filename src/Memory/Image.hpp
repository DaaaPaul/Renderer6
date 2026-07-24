#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include "Backend/LogicalDevice.h"
#include "Utility/Utility.h"

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
	~Image() {
		vkDestroyImage(g_device, image, nullptr);
	}

	VkMemoryRequirements get_memory_requirements() const;

	static std::vector<VkImage> get_vk_images(const std::vector<Image*>& p_images);
};
