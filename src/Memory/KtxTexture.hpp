#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include <ktx.h>
#include "Image.hpp"
#include "utility/Vulkan.h"

struct KtxTexture {
	ktxTexture2* ktx_texture{};
	Image image;

	explicit KtxTexture(const char* ktx_path,
					 uint32_t mip_level_count, 
					 uint32_t array_layer_count, 
					 VkSampleCountFlagBits sample_count, 
					 VkSharingMode sharing_mode,
					 const std::vector<uint32_t>& queue_family_indices);
	void destroy() {
		image.destroy();
		ktxTexture_Destroy(ktxTexture(ktx_texture));
	}

	void copy_texture_data_to_image()  {
		Image::copy_to_image(image, ktx_texture->pData, VkOffset3D{0, 0, 0}, VkExtent3D{ktx_texture->baseWidth, ktx_texture->baseHeight, 1}, 0);
	}
};
