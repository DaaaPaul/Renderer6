#pragma once

#include <vulkan/vulkan.h>
#include <ktx.h>
#include "Resource.hpp"
#include "Image.hpp"

struct KtxTexture {
	ktxTexture2* p_ktx_texture{};
	explicit KtxTexture(ktxTexture2* p_ktx_texture) :
		p_ktx_texture{ p_ktx_texture } {
		
	}
};

class Texture : public KtxTexture, public Resource, public Image {
	public:
	explicit Texture(uint32_t name_index, 
					 const char* ktx_path,
					 uint32_t mip_level_count, 
					 uint32_t array_layer_count, 
					 VkSampleCountFlagBits sample_count, 
					 VkSharingMode sharing_mode,
					 const std::vector<uint32_t>& queue_family_indices);
	void destroy() noexcept override;

	static VkResult copy_ktx_texture_to_image(VkImage image, const ktxTexture2* p_ktx_texture);
};
