#include <fstream>
#include "Backend/ShaderModule.h"
#include "Backend/LogicalDevice.h"
#include "Utility/Utility.h"

namespace ShaderModule {
	void init() {
		g_shader_modules.push_back(Vulkan::create_shader_module(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\shaders.spv)"));
	}

	void clear() {
		for(VkShaderModule shaderModule : g_shader_modules) {
			vkDestroyShaderModule(g_device, shaderModule, nullptr);
		}
		g_shader_modules.clear();
	}
}
