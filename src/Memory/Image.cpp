#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include "Image.hpp"
#include "Backend/LogicalDevice.h"
#include "Utility/Vulkan.h"

Image::Image(VkImageCreateFlags create_flags, 
			 VkImageType image_type,    
			 VkFormat format, 
			 VkExtent3D extent, 
		 	 uint32_t mip_level_count,
		 	 uint32_t array_layer_count,
		 	 VkSampleCountFlagBits sample_count,
		 	 VkImageUsageFlags usage,
		 	 VkSharingMode sharing_mode, 
		 	 const std::vector<uint32_t>& queue_family_indices) {
	VkImageCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.flags = create_flags,
		.imageType = image_type,
		.format = format,
		.extent = extent,
		.mipLevels = mip_level_count,
		.arrayLayers = array_layer_count,
		.samples = sample_count,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = usage,
		.sharingMode = sharing_mode,
		.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size()),
		.pQueueFamilyIndices = queue_family_indices.data(),
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	Vulkan::check(vkCreateImage(g_device, &create, nullptr, &image), "Image: failed to create image");
}

VkMemoryRequirements Image::get_memory_requirements() const { 
	VkMemoryRequirements memory_requirements{};
	vkGetImageMemoryRequirements(g_device, image, &memory_requirements);
	
	return memory_requirements; 
}

std::vector<VkImage> Image::get_vk_images(const std::vector<Image*>& p_images) {
	std::vector<VkImage> vk_images(p_images.size());

	for(int i = 0; i < p_images.size(); ++i) {
		vk_images[i] = p_images[i]->image;
	}

	return vk_images;
}