#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Backend/LogicalDevice.h"

class Image {
	private:
	VkImage image{};
	VkImageView image_view{};

	public:
	virtual ~Image() = default;
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
				   VkImageViewCreateFlags image_view_create_flags = VK_IMAGE_VIEW_CREATE_FLAG_BITS_MAX_ENUM,
				   VkImageSubresourceRange image_view_subresource_range = VkImageSubresourceRange{ .aspectMask = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM});
	virtual void destroy() noexcept;

	VkImage get_image() const { 
		return image; 
	}

	VkImageView get_image_view() const { 
		return image_view; 
	}

	VkMemoryRequirements get_memory_requirements() const { 
		VkMemoryRequirements memory_requirements{};
		vkGetImageMemoryRequirements(g_device, image, &memory_requirements);
	
		return memory_requirements; 
	}
};
