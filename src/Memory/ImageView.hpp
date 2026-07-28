#pragma once

#include <vulkan/vulkan_core.h>
#include "backend/LogicalDevice.h"

class ImageView {
	private:
	VkImageView image_view{};

	public:
	explicit ImageView(VkImageViewCreateFlags create_flags,
					   VkImage image,
					   VkImageViewType view_type,
					   VkFormat format,
					   VkImageSubresourceRange image_subresource_range);
	void destroy() {
		vkDestroyImageView(g_device, image_view, nullptr);
	}

	VkImageView get_image_view() const {
		return image_view;
	}
};