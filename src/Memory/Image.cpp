#include "Image.hpp"
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
		.queueFamilyIndexCount = UINT32(queue_family_indices.size()),
		.pQueueFamilyIndices = queue_family_indices.data(),
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};
	VK_CHECK(vkCreateImage(g_device, &create, nullptr, &image), "Image: failed to create image")
}

void Image::destroy() noexcept {
	vkDestroyImage(g_device, image, nullptr);
}