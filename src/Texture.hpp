#pragma once

#include <vulkan/vulkan.h>
#include <ktx.h>
#include "Resource.hpp"

namespace Resource {
	class Texture : public Resource {
		private:
		VkImage image{};
		VkImageView imageView{};
		VkSampler sampler{};

		VkMemoryRequirements requirements{};

		ktxTexture2* texture{};

		public:
		explicit Texture(uint32_t idx, const char* texturePath);
		~Texture();

		VkMemoryRequirements getRequirements() const { return requirements; }
		VkImageView getImageView() const { return imageView; }

		private:
		static ktxTexture2* getKtx(const char* ktxPath);
		static VkImage createImage(ktxTexture2* ktxTexture);
		static VkImageView createImageView(VkImage image, VkFormat format);
		static VkSampler createSampler();
	};
}