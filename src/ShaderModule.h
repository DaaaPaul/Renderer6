#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace ShaderModule {
	inline std::vector<VkShaderModule> g_shader_modules{};

	void init();
	void clear();
}
