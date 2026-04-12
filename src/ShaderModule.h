#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Engine {
	namespace ShaderModule {
		inline std::vector<VkShaderModule> gShaderModules{};

		void add();
		VkShaderModule newShaderModule(VkShaderModuleCreateInfo const&);
		void clear() noexcept;
	}
}