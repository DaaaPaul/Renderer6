#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include "Image.hpp"
#include "backend/LogicalDevice.h"
#include "utility/Vulkan.h"

Image::Image(VkImageCreateFlags create_flags, 
			 VkImageType image_type,    
			 VkFormat format, 
			 VkExtent3D extent, 
		 	 uint32_t mip_level_count,
		 	 uint32_t array_layer_count,
		 	 VkSampleCountFlagBits sample_count,
		 	 VkImageUsageFlags usage,
		 	 VkSharingMode sharing_mode, 
		 	 const std::vector<uint32_t>& queue_family_indices,
			 VmaAllocationCreateInfo vma_allocation_info) {
	VkImageCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.flags = create_flags,
		.imageType = image_type,
		.format = format,
		.extent = extent,
		.mipLevels = mip_level_count,
		.arrayLayers = array_layer_count,
		.samples = sample_count,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = usage,
		.sharingMode = sharing_mode,
		.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size()),
		.pQueueFamilyIndices = queue_family_indices.data(),
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	Vulkan::check(vmaCreateImage(Vulkan::g_vma_allocator, &create, &vma_allocation_info, &image, &allocation, &allocation_info), "Image: failed to create image");
}

void Image::transition_layout(Image image, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mip_level_count, uint32_t array_layer_count) {
	VkHostImageLayoutTransitionInfo transition{
		.sType = VK_STRUCTURE_TYPE_HOST_IMAGE_LAYOUT_TRANSITION_INFO,
		.image = image.image,
		.oldLayout = old_layout,
		.newLayout = new_layout,
		.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, mip_level_count, 0, array_layer_count}
	};
	Vulkan::check(Vulkan::vkTransitionImageLayoutEXT(g_device, 1, &transition), "transition_layout: transition failed");
}

void Image::copy_to(Image image, const void* p_data, VkOffset3D offset, VkExtent3D extent, uint32_t mip_level) {
	VkMemoryToImageCopyEXT region{
		.sType = VK_STRUCTURE_TYPE_MEMORY_TO_IMAGE_COPY_EXT,
		.pHostPointer = p_data,
		.imageSubresource = VkImageSubresourceLayers{VK_IMAGE_ASPECT_COLOR_BIT, mip_level, 0, 1},
		.imageExtent = extent
	};

	VkCopyMemoryToImageInfo copy{
		.sType = VK_STRUCTURE_TYPE_COPY_MEMORY_TO_IMAGE_INFO_EXT,
		.dstImage = image.image,
		.dstImageLayout = VK_IMAGE_LAYOUT_GENERAL,
		.regionCount = 1,
		.pRegions = &region,
	};

	Vulkan::check(Vulkan::vkCopyMemoryToImageEXT(g_device, &copy), "copy_to: copy failed");
}