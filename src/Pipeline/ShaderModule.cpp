#include <fstream>
#include "pipeline/ShaderModule.h"
#include "backend/LogicalDevice.h"
#include "utility/Vulkan.h"

namespace ShaderModule {
	void init() {
		g_shader_modules.push_back(Vulkan::create_shader_module(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\pbr.spv)"));
		g_shader_modules.push_back(Vulkan::create_shader_module(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\simple.spv)"));
		g_shader_modules.push_back(Vulkan::create_shader_module(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\gui.spv)"));
	}

	void clear() {
		for(VkShaderModule shaderModule : g_shader_modules) {
			vkDestroyShaderModule(g_device, shaderModule, nullptr);
		}
		g_shader_modules.clear();
	}
}
