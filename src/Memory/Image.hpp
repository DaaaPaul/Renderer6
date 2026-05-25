#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Backend/LogicalDevice.h"

class Image {
	private:
	VkImage image{};

	public:
	virtual ~Image() = default;
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
				   const std::vector<uint32_t>& queue_family_indices);
	virtual void destroy() noexcept;

	VkImage get_image() const { 
		return image; 
	}

	VkMemoryRequirements get_memory_requirements() const { 
		VkMemoryRequirements memory_requirements{};
		vkGetImageMemoryRequirements(g_device, image, &memory_requirements);
	
		return memory_requirements; 
	}

	static std::vector<VkImage> get_vk_images(const std::vector<Image*>& p_images) {
		std::vector<VkImage> vk_images(p_images.size());

		for(int i = 0; i < p_images.size(); ++i) {
			vk_images[i] = p_images[i]->get_image();
		}

		return vk_images;
	}
};
