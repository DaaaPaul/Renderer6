#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace PipelineLayouts {
	inline std::vector<VkPipelineLayout> gLayouts{};

	void add();
	VkPipelineLayout newLayout(VkPipelineLayoutCreateInfo const&);
	void clear();
}
