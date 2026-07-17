#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include <ktx.h>
#include "Image.hpp"
#include "Utility/Vulkan.h"

struct KtxTexture {
	ktxTexture2* ktx_texture{};
	explicit KtxTexture(ktxTexture2* ktx_texture, ktx_transcode_fmt_e target_format) :
		ktx_texture{ ktx_texture } {
		Vulkan::transcode_ktx_texture(this->ktx_texture, target_format);
	}
};

class Texture : public KtxTexture, public Image {
	public:
	explicit Texture(const char* ktx_path,
					 uint32_t mip_level_count, 
					 uint32_t array_layer_count, 
					 VkSampleCountFlagBits sample_count, 
					 VkSharingMode sharing_mode,
					 const std::vector<uint32_t>& queue_family_indices);
	~Texture() noexcept override {
		ktxTexture_Destroy(ktxTexture(KtxTexture::ktx_texture));
	}

	ktxTexture2* get_ktx_texture() const {
		return KtxTexture::ktx_texture;
	}

	static void copy_ktx_texture_to_image(VkImage image, const ktxTexture2* ktx_texture);
};
