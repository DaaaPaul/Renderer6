#include "ImageView.hpp"
#include "Utility/Utility.h"
#include "Backend/LogicalDevice.h"

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
	VK_CHECK(vkCreateImageView(g_device, &create, nullptr, &image_view), "ImageView: failed")
}

void ImageView::destroy() noexcept {
	vkDestroyImageView(g_device, image_view, nullptr);
}