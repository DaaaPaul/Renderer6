#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Engine {
	namespace ShaderModule {
		std::vector<VkShaderModule> shaderModules{};

		void populate();
		VkShaderModule newShaderModule(VkShaderModuleCreateInfo const&);
		void clear() noexcept;
	}
}