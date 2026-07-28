#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "utility/Utility.h"

namespace Pipelines {
	inline std::vector<VkPipeline> g_pipelines{};

	void init();
	void clear();
}
