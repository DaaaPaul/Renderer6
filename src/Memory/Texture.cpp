#include <stdexcept>
#include "Texture.hpp"
#include "Backend/PhysicalDevice.h"
#include "Backend/LogicalDevice.h"
#include "Utility/Utility.h"
#include "Utility/Load.h"

Texture::Texture(uint32_t name_index, const char* texturePath) : 
	Resource(name_index), p_ktx_texture{ get_ktx_texture(texturePath) }, image{ create_image(p_ktx_texture) }, 
	image_view{ create_image_view(image, static_cast<VkFormat>(p_ktx_texture->vkFormat)) }, sampler{ create_sampler() } {
	
	vkGetImageMemoryRequirements(g_device, image, &memory_requirements);
}

void Texture::destroy() noexcept {
	vkDestroySampler(g_device, sampler, nullptr);
	vkDestroyImageView(g_device, image_view, nullptr);
	vkDestroyImage(g_device, image, nullptr);
	ktxTexture_Destroy(ktxTexture(p_ktx_texture));
}

VkResult Texture::copy(VkImage image, const ktxTexture2* p_ktx_texture) {
	VkHostImageLayoutTransitionInfo transition{
		.sType = VK_STRUCTURE_TYPE_HOST_IMAGE_LAYOUT_TRANSITION_INFO,
		.image = image,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_GENERAL,
		.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
	};
	VK_CHECK(Load::vkTransitionImageLayoutEXT(g_device, 1, &transition), "copyToImage: transition error")

	VkMemoryToImageCopyEXT region{
		.sType = VK_STRUCTURE_TYPE_MEMORY_TO_IMAGE_COPY_EXT,
		.pHostPointer = p_ktx_texture->pData,
		.imageSubresource = VkImageSubresourceLayers{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
		.imageExtent = VkExtent3D{p_ktx_texture->baseWidth, p_ktx_texture->baseHeight, p_ktx_texture->baseDepth}
	};

	VkCopyMemoryToImageInfoEXT copy{
		.sType = VK_STRUCTURE_TYPE_COPY_MEMORY_TO_IMAGE_INFO_EXT,
		.dstImage = image,
		.dstImageLayout = VK_IMAGE_LAYOUT_GENERAL,
		.regionCount = 1,
		.pRegions = &region,
	};

	return vkCopyMemoryToImage(g_device, &copy);
}

ktxTexture2* Texture::get_ktx_texture(const char* ktxPath) {
	ktxTexture2* p_ktx_texture{};
	KTX_error_code error = ktxTexture2_CreateFromNamedFile(ktxPath, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &p_ktx_texture);

	if(error != KTX_SUCCESS) {
		throw std::runtime_error("get_ktx_texture: load failure");
	} else {
		if(ktxTexture2_NeedsTranscoding(p_ktx_texture)) {
			constexpr ktx_transcode_fmt_e TARGET_FORMAT = KTX_TTF_BC7_RGBA;

			if(ktxTexture2_TranscodeBasis(p_ktx_texture, TARGET_FORMAT, 0) != KTX_SUCCESS) {
				throw std::runtime_error("get_ktx_texture: compress failure");
			}
		}
	}

	return p_ktx_texture;
}

VkImage Texture::create_image(ktxTexture2* p_ktx_texture) {
	VkImage image{};

	VkImageCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = static_cast<VkFormat>(p_ktx_texture->vkFormat),
		.extent = VkExtent3D(p_ktx_texture->baseWidth, p_ktx_texture->baseHeight, p_ktx_texture->baseDepth),
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT),
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VK_CHECK(vkCreateImage(g_device, &create, nullptr, &image), "create_image: failed")

	return image;
}

VkImageView Texture::create_image_view(VkImage image, VkFormat format) {
	VkImageView view{};

	VkImageViewCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
	};

	VK_CHECK(vkCreateImageView(g_device, &create, nullptr, &view), "create_image_view: failed")

	return view;
}

VkSampler Texture::create_sampler() {
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
		.maxAnisotropy = PhysicalDevice::g_limits.maxSamplerAnisotropy,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
	};

	VK_CHECK(vkCreateSampler(g_device, &create, nullptr, &sampler), "create_sampler: failed")

	return sampler;
}
