#include "Image.hpp"

Image::Image(VkImageCreateFlags create_flags, 
		     VkImageType image_type,    
			 VkFormat format, 
			 VkExtent3D extent, 
			 uint32_t mip_level_count,
			 uint32_t array_layer_count,
			 VkSampleCountFlagBits sample_count,
			 VkImageUsageFlags usage,
			 VkSharingMode sharing_mode, 
			 const std::vector<uint32_t>& queue_family_indices,
			 VkImageViewCreateFlags image_view_create_flags,
			 VkImageSubresourceRange image_view_subresource_range) {
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

	if(!(image_view_create_flags == VK_IMAGE_VIEW_CREATE_FLAG_BITS_MAX_ENUM && image_view_subresource_range.aspectMask == VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM)) {
		VkImageViewCreateInfo image_view_create{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = image,
			.viewType = static_cast<VkImageViewType>(image_type),
			.format = format,
			.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 },
		};

		VK_CHECK(vkCreateImageView(g_device, &image_view_create, nullptr, &image_view), "Image: failed to create image view")
	} else {
		image_view = VK_NULL_HANDLE;
	}
}

void Image::destroy() noexcept {
	vkDestroyImage(g_device, image, nullptr);

	if(image_view != VK_NULL_HANDLE) {
		vkDestroyImageView(g_device, image_view, nullptr);
	}
}