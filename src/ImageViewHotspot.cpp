#include "ImageViewHotspot.h"
#include "LogicalDevice.h"

namespace ImageViewHotspot {
	VkImageView newView(VkImageViewCreateInfo const& CREATE) {
		VkImageView newView{};

		VK_CHECK(vkCreateImageView(g_device, &CREATE, nullptr, &newView), "Failed to create image view")
		views.push_back(newView);

		return newView;
	}

	void pop() {
		vkDestroyImageView(g_device, views[views.size() - 1], nullptr);
		views.pop_back();
	}

	void clear() {
		for(VkImageView view : views) {
			vkDestroyImageView(g_device, view, nullptr);
		}
	}
}