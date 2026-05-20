#include <stdexcept>
#include "Texture.hpp"
#include "Backend/PhysicalDevice.h"
#include "Backend/LogicalDevice.h"
#include "Utility/Utility.h"
#include "Utility/Load.h"

Texture::Texture(uint32_t name_index,
				 const char* ktx_path,
				 VkFormat format,
				 uint32_t width,
				 uint32_t height,
				 uint32_t mip_level_count,
				 uint32_t array_layer_count,
				 VkSampleCountFlagBits sample_count,
				 VkSharingMode sharing_mode,
				 const std::vector<uint32_t>& queue_family_indices) : 
	Resource(name_index), 
	Image(VK_NO_FLAGS,
		  VK_IMAGE_TYPE_2D,
		  format,
		  VkExtent3D{ width, height, 1 },
		  mip_level_count,
		  array_layer_count,
		  sample_count,
		  VK_IMAGE_USAGE_SAMPLED_BIT,
		  sharing_mode,
		  queue_family_indices,
		  VK_NO_FLAGS,
		  VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }), 
	p_ktx_texture{ Utility::get_ktx_texture(ktx_path) } {

}

void Texture::destroy() noexcept {
	vkDestroyImageView(g_device, get_image_view(), nullptr);
	vkDestroyImage(g_device, get_image(), nullptr);
	ktxTexture_Destroy(ktxTexture(p_ktx_texture));
}

VkResult Texture::copy_into_image(VkImage image, const ktxTexture2* p_ktx_texture) {
	VkHostImageLayoutTransitionInfo transition{
		.sType = VK_STRUCTURE_TYPE_HOST_IMAGE_LAYOUT_TRANSITION_INFO,
		.image = image,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_GENERAL,
		.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
	};
	VK_CHECK(Load::vkTransitionImageLayoutEXT(g_device, 1, &transition), "copyToImage: transition error")

	VkMemoryToImageCopyEXT region{
		.sType = VK_STRUCTURE_TYPE_MEMORY_TO_IMAGE_COPY_EXT,
		.pHostPointer = p_ktx_texture->pData,
		.imageSubresource = VkImageSubresourceLayers{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
		.imageExtent = VkExtent3D{p_ktx_texture->baseWidth, p_ktx_texture->baseHeight, p_ktx_texture->baseDepth}
	};

	VkCopyMemoryToImageInfoEXT copy{
		.sType = VK_STRUCTURE_TYPE_COPY_MEMORY_TO_IMAGE_INFO_EXT,
		.dstImage = image,
		.dstImageLayout = VK_IMAGE_LAYOUT_GENERAL,
		.regionCount = 1,
		.pRegions = &region,
	};

	return vkCopyMemoryToImage(g_device, &copy);
}