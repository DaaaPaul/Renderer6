#pragma once

#include <vulkan/vulkan_core.h>
#include "Backend/LogicalDevice.h"

class Sampler {
	private:
	VkSampler sampler{};

	public:
	Sampler() = default;

	explicit Sampler(VkSamplerCreateFlags create_flags,
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
					 VkBorderColor border_color);
	~Sampler() {
		vkDestroySampler(g_device, sampler, nullptr);
	}

	VkSampler get_sampler() const {
		return this->sampler;
	}
};