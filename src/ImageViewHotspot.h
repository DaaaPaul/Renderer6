#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Util.h"

namespace ImageViewHotspot {
	inline std::vector<VkImageView> views{};

	VkImageView newView(VkImageViewCreateInfo const&);
	void pop();
	void clear();
}