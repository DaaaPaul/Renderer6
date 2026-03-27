#include <vulkan/vulkan.h>
#include <vector>
#include "Util.h"

namespace ImageViewHotspot {
	std::vector<VkImageView> views{};

	VkImageView newView(VkImageViewCreateInfo const&);
	void clear() noexcept;
}