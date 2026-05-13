#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Util.h"

namespace ImageViewHotspot {
	inline std::vector<VkImageView> g_image_views{};

	VkImageView newView(VkImageViewCreateInfo const& create);
	void pop();
	void clear();
}