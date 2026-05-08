#pragma once

#include <vulkan/vulkan.h>
#include <ktx.h>
#include "Resource.hpp"

class Texture : public Resource {
	private:
	VkImage image{};
	VkImageView imageView{};
	VkSampler sampler{};

	VkMemoryRequirements memory_requirements{};

	ktxTexture2* texture{};

	public:
	explicit Texture(uint32_t idx, const char* texturePath);
	~Texture();

	VkImage get_image() const { return image; }
	VkImageView get_image_view() const { return imageView; }
	VkMemoryRequirements get_memory_requirements() const { return memory_requirements; }

	void copyToImage();

	private:
	static ktxTexture2* getKtx(const char* ktxPath);
	static VkImage create_image(ktxTexture2* ktxTexture);
	static VkImageView create_image_view(VkImage image, VkFormat format);
	static VkSampler createSampler();
};
