#include "ImageViewHotspot.h"
#include "LogicalDevice.h"

namespace ImageViewHotspot {
	VkImageView newView(VkImageViewCreateInfo const& CREATE) {
		VkImageView newView{};

		CHECK_VK_SUCCESS(vkCreateImageView(gDevice, &CREATE, nullptr, &newView), "Failed to create image view")
		views.push_back(newView);

		return newView;
	}

	void pop() {
		vkDestroyImageView(gDevice, views[views.size() - 1], nullptr);
		views.pop_back();
	}

	void clear() {
		for(VkImageView view : views) {
			vkDestroyImageView(gDevice, view, nullptr);
		}
	}
}