#include <vulkan/vulkan_core.h>
#include "ImageView.hpp"
#include "utility/Vulkan.h"
#include "backend/LogicalDevice.h"

ImageView::ImageView(VkImageViewCreateFlags create_flags,
					 VkImage image,
					 VkImageViewType view_type,
					 VkFormat format,
					 VkImageSubresourceRange image_subresource_range) {
	
	VkImageViewCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.flags = create_flags,
		.image = image,
		.viewType = view_type,
		.format = format,
		.subresourceRange = image_subresource_range
	};
	Vulkan::check(vkCreateImageView(g_device, &create, nullptr, &image_view), "ImageView: failed");
}