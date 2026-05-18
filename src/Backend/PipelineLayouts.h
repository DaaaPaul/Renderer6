#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace PipelineLayouts {
	inline std::vector<VkPipelineLayout> g_layouts{};

	void init();
	void clear();
}
