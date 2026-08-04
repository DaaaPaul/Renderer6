#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <cstdint>
#include <ktx.h>
#include "Image.hpp"
#include "utility/Vulkan.h"

class KtxTexture {
	public:
	KtxTexture() = default;
	explicit KtxTexture(const char* ktx_path,
					    ktx_transcode_fmt_e transcode_format,
					    uint32_t mip_level_count, 
					    uint32_t array_layer_count, 
					    VkSampleCountFlagBits sample_count, 
					    VkSharingMode sharing_mode,
					    const std::vector<uint32_t>& queue_family_indices,
					    VmaAllocationCreateInfo vma_allocation_info);
	void destroy() {
		image.destroy();
		ktxTexture_Destroy(ktxTexture(ktx_texture));
	}

	Image get_image() const {
		return image;
	}
	ktxTexture2* get_ktx_texture() const {
		return ktx_texture;
	}

	void copy_texture_data_to_image()  {
		Image::copy_to(image, ktx_texture->pData, VkOffset3D{0, 0, 0}, VkExtent3D{ktx_texture->baseWidth, ktx_texture->baseHeight, 1}, 0);
	}

	private:
	ktxTexture2* ktx_texture{};
	Image image;
};