#include "ImageViewHotspot.h"
#include "LogicalDevice.h"

namespace ImageViewHotspot {
	VkImageView newView(VkImageViewCreateInfo const& CREATE) {
		VkImageView newView{};

		CHECK_VK_SUCCESS(vkCreateImageView(gpDevice, &CREATE, nullptr, &newView), "Failed to create image view")
		views.push_back(newView);

		return newView;
	}

	void clear() noexcept {
		for(VkImageView& view : views) {
			vkDestroyImageView(gpDevice, view, nullptr);
		}
	}
}