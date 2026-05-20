#pragma once

#include "Image.hpp"

class DepthImage : public Image{
	public:
	explicit DepthImage(VkFormat format, 
						uint32_t width,
						uint32_t height,
						VkSharingMode sharing_mode, 
						const std::vector<uint32_t>& queue_family_indices);
};
