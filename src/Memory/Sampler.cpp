#include "Sampler.hpp"
#include "Utility/Vulkan.h"

Sampler::Sampler(VkSamplerCreateFlags create_flags,
				 VkFilter mag_filter,
				 VkFilter min_filter,
				 VkSamplerMipmapMode mipmap_mode,
				 VkSamplerAddressMode address_mode_u,
				 VkSamplerAddressMode address_mode_v,
				 VkSamplerAddressMode address_mode_w,
				 float mip_lod_bias,
				 VkBool32 enable_anisotropy,
				 float max_anisotropy,
				 VkBool32 enable_compare,
				 VkCompareOp compare_op,
				 float min_lod,
				 float max_lod,
				 VkBorderColor border_color) {
	
	VkSamplerCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.flags = create_flags,
		.magFilter = mag_filter,
		.minFilter = min_filter,
		.mipmapMode = mipmap_mode,
		.addressModeU = address_mode_u,
		.addressModeV = address_mode_v,
		.addressModeW = address_mode_w,
		.mipLodBias = mip_lod_bias,
		.anisotropyEnable = enable_anisotropy,
		.maxAnisotropy = max_anisotropy,
		.compareEnable = enable_compare,
		.compareOp = compare_op,
		.minLod = min_lod,
		.maxLod = max_lod,
		.borderColor = border_color,
		.unnormalizedCoordinates = VK_FALSE,
	};

	Vulkan::check(vkCreateSampler(g_device, &create, nullptr, &sampler), "Sampler: failed");
}
	