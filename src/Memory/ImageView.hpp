#pragma once

#include <vulkan/vulkan.h>

class ImageView {
	private:
	VkImageView image_view{};

	public:
	ImageView() = default;
	explicit ImageView(VkImageViewCreateFlags create_flags,
					   VkImage image,
					   VkImageViewType view_type,
					   VkFormat format,
					   VkImageSubresourceRange image_subresource_range);
	void destroy() noexcept;

	VkImageView get_image_view() const {
		return image_view;
	}
};