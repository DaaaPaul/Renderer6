#include "ImageViewHotspot.h"
#include "LogicalDevice.h"

namespace ImageViewHotspot {
	VkImageView newView(VkImageViewCreateInfo const& create) {
		VkImageView view{};

		VK_CHECK(vkCreateImageView(g_device, &create, nullptr, &view), "Failed to create image view")
		g_image_views.push_back(view);

		return view;
	}

	void pop() {
		vkDestroyImageView(g_device, g_image_views[g_image_views.size() - 1], nullptr);
		g_image_views.pop_back();
	}

	void clear() {
		for(VkImageView view : g_image_views) {
			vkDestroyImageView(g_device, view, nullptr);
		}
	}
}