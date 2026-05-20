#include "DepthImage.hpp"
#include "Utility/Utility.h"

DepthImage::DepthImage(VkFormat format, 
					   uint32_t width,
					   uint32_t height,
					   VkSharingMode sharing_mode, 
					   const std::vector<uint32_t>& queue_family_indices) :
	Image(VK_NO_FLAGS, 
		  VK_IMAGE_TYPE_2D, 
		  format, 
		  VkExtent3D{ width, height, 1 }, 
		  1, 
		  1, 
		  VK_SAMPLE_COUNT_1_BIT, 
		  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		  sharing_mode, 
		  queue_family_indices,
		  VK_NO_FLAGS,
		  VkImageSubresourceRange{ VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 }) {
		
}