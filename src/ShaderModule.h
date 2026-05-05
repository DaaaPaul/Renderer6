#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace ShaderModule {
	inline std::vector<VkShaderModule> gShaderModules{};

	std::vector<char> getBytes(std::string const& PATH);

	void add();
	VkShaderModule newShaderModule(VkShaderModuleCreateInfo const&);
	void clear();
}
