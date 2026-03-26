#include "ImageViewHotspot.h"
#include "LogicalDevice.h"

namespace ImageViewHotspot {
	VkImageView newView(VkImageViewCreateInfo const& CREATE) {
		VkImageView newView{};

		CHECK_VK_SUCCESS(vkCreateImageView(Backend::LogicalDevice::gpDevice, &CREATE, nullptr, &newView), "Failed to create image view")
		views.push_back(newView);

		return newView;
	}

	void emptyViews() noexcept {
		for(VkImageView& view : views) {
			vkDestroyImageView(Backend::LogicalDevice::gpDevice, view, nullptr);
		}
	}
}