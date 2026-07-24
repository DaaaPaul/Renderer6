#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include <ktx.h>
#include "Image.hpp"
#include "Utility/Vulkan.h"

struct Texture {
	ktxTexture2* ktx_texture{};
	Image image;

	explicit Texture(const char* ktx_path,
					 uint32_t mip_level_count, 
					 uint32_t array_layer_count, 
					 VkSampleCountFlagBits sample_count, 
					 VkSharingMode sharing_mode,
					 const std::vector<uint32_t>& queue_family_indices);
	~Texture() {
		ktxTexture_Destroy(ktxTexture(ktx_texture));
	}

	static void copy_ktx_texture_to_image(VkImage image, const ktxTexture2* ktx_texture);
};
