#pragma once

#include <vulkan/vulkan.h>
#include <ktx.h>
#include "Resource.hpp"

class Texture : public Resource {
	private:
	ktxTexture2* p_ktx_texture{};

	VkImage image{};
	VkImageView image_view{};
	VkSampler sampler{};

	VkMemoryRequirements memory_requirements{};

	public:
	explicit Texture(uint32_t index, const char* texturePath);
	~Texture();

	ktxTexture2* get_ktx_texture() const { return p_ktx_texture; }
	VkImage get_image() const { return image; }
	VkImageView get_image_view() const { return image_view; }
	VkMemoryRequirements get_memory_requirements() const { return memory_requirements; }

	static VkResult copy(VkImage image, const ktxTexture2* p_ktx_texture);

	private:
	static ktxTexture2* get_ktx_texture(const char* ktxPath);
	static VkImage create_image(ktxTexture2* ktxTexture);
	static VkImageView create_image_view(VkImage image, VkFormat format);
	static VkSampler create_sampler();
};
