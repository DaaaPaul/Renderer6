#include <vulkan/vulkan_core.h>
#include <ktx.h>
#include <vector>
#include <cstdint>
#include "Image.hpp"
#include "KtxTexture.hpp"
#include "backend/LogicalDevice.h"
#include "utility/Vulkan.h"

KtxTexture::KtxTexture(const char* ktx_path,
				 uint32_t mip_level_count,
				 uint32_t array_layer_count,
				 VkSampleCountFlagBits sample_count,
				 VkSharingMode sharing_mode,
				 const std::vector<uint32_t>& queue_family_indices) : 
	ktx_texture{ Vulkan::load_ktx_texture(ktx_path, KTX_TTF_BC7_RGBA) },
	image(Vulkan::NO_FLAGS,
		  VK_IMAGE_TYPE_2D,
		  static_cast<VkFormat>(ktx_texture->vkFormat),
		  VkExtent3D{ktx_texture->baseWidth, ktx_texture->baseHeight, 1},
		  mip_level_count,
		  array_layer_count,
		  sample_count,
		  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_HOST_TRANSFER_BIT,
		  sharing_mode,
		  queue_family_indices) {

}