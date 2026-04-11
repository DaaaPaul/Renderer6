#include "ShaderModule.h"
#include "LogicalDevice.h"
#include "Util.h"

namespace Engine {
	namespace ShaderModule {
		void populate() {
			std::vector<char> modelShadersBytes(Util::getFileBytes(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\shaders.spv)"));
			VkShaderModuleCreateInfo modelShadersCreate{
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = UINT32(modelShadersBytes.size()),
				.pCode = reinterpret_cast<uint32_t const*>(modelShadersBytes.data())
			};
			newShaderModule(modelShadersCreate);

			std::vector<char> particlesShadersBytes(Util::getFileBytes(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\particleShaders.spv)"));
			VkShaderModuleCreateInfo particleShadersCreate{
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = UINT32(particlesShadersBytes.size()),
				.pCode = reinterpret_cast<uint32_t const*>(particlesShadersBytes.data())
			};
			newShaderModule(particleShadersCreate);
		}

		VkShaderModule newShaderModule(VkShaderModuleCreateInfo const& CREATE) {
			VkShaderModule shaderModule{};

			CHECK_VK_SUCCESS(vkCreateShaderModule(gpDevice, &CREATE, nullptr, &shaderModule), "Failed to create shader module")
			shaderModules.push_back(shaderModule);

			return shaderModule;
		}

		void clear() noexcept {
			for(VkShaderModule& shaderModule : shaderModules) {
				vkDestroyShaderModule(gpDevice, shaderModule, nullptr);
			}
		}
	}
}