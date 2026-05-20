#pragma once

#include <vulkan/vulkan.h>
#include <ktx.h>
#include "Resource.hpp"
#include "Image.hpp"

class Texture : public Resource, public Image {
	private:
	ktxTexture2* p_ktx_texture{};

	public:
	explicit Texture(uint32_t name_index, 
					 const char* ktx_path,
					 VkFormat format,
					 uint32_t width,
					 uint32_t height,
					 uint32_t mip_level_count, 
					 uint32_t array_layer_count, 
					 VkSampleCountFlagBits sample_count, 
					 VkSharingMode sharing_mode,
					 const std::vector<uint32_t>& queue_family_indices);
	void destroy() noexcept override;

	static VkResult copy_into_image(VkImage image, const ktxTexture2* p_ktx_texture);
};
