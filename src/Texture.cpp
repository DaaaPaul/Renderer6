#include <stdexcept>
#include "Texture.hpp"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "Util.h"

namespace Resource {
	Texture::Texture(uint32_t idx, const char* texturePath) : Resource(idx) {
		texture = getKtx(texturePath);
		image = createImage(texture);
		
		vkGetImageMemoryRequirements(gDevice, image, &requirements);

		imageView = createImageView(image, static_cast<VkFormat>(texture->vkFormat));

		sampler = createSampler();
	}

	Texture::~Texture() {
		vkDestroySampler(gDevice, sampler, nullptr);
		vkDestroyImageView(gDevice, imageView, nullptr);
		vkDestroyImage(gDevice, image, nullptr);
		ktxTexture_Destroy(ktxTexture(texture));
	}

	ktxTexture2* Texture::getKtx(const char* ktxPath) {
		ktxTexture2* t{};
		KTX_error_code error = ktxTexture2_CreateFromNamedFile(ktxPath, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &t);

		if(error != KTX_SUCCESS) {
			throw std::runtime_error("getKtx: load failure");
		} else {
			if(ktxTexture2_NeedsTranscoding(t)) {
				constexpr ktx_transcode_fmt_e TARGET_FORMAT = KTX_TTF_BC7_RGBA;

				if(ktxTexture2_TranscodeBasis(t, TARGET_FORMAT, 0) != KTX_SUCCESS) {
					throw std::runtime_error("getKtx: compress failure");
				}
			}
		}

		return t;
	}

	VkImage Texture::createImage(ktxTexture2* texture) {
		VkImage image{};

		VkImageCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = static_cast<VkFormat>(texture->vkFormat),
			.extent = VkExtent3D(texture->baseWidth, texture->baseHeight, texture->baseDepth),
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &Backend::PhysicalDevice::gQueueFamilyIndices[0],
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};

		VK_CHECK(vkCreateImage(gDevice, &create, nullptr, &image), "createImage: failed")

		return image;
	}

	VkImageView Texture::createImageView(VkImage image, VkFormat format) {
		VkImageView view{};

		VkImageViewCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = format,
			.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
		};

		VK_CHECK(vkCreateImageView(gDevice, &create, nullptr, &view), "createImageView: failed")

		return view;
	}

	VkSampler Texture::createSampler() {
		VkSampler sampler{};

		VkSamplerCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.anisotropyEnable = VK_TRUE,
			.maxAnisotropy = Backend::PhysicalDevice::gLimits.maxSamplerAnisotropy,
			.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		};

		VK_CHECK(vkCreateSampler(gDevice, &create, nullptr, &sampler), "createSampler: failed")

		return sampler;
	}
}