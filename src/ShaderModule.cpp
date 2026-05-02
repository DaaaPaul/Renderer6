#include "ShaderModule.h"
#include "LogicalDevice.h"
#include "Util.h"

namespace Engine {
	namespace ShaderModule {
		void add() {
			std::vector<char> modelVertFragBytes(Util::getFileBytes(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\modelVertFrag.spv)"));
			VkShaderModuleCreateInfo modelVertFragCreate{
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = UINT32(modelVertFragBytes.size()),
				.pCode = reinterpret_cast<uint32_t const*>(modelVertFragBytes.data())
			};
			newShaderModule(modelVertFragCreate);

			std::vector<char> particleVertFragBytes(Util::getFileBytes(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\particleVertFrag.spv)"));
			VkShaderModuleCreateInfo particleVertFragCreate{
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = UINT32(particleVertFragBytes.size()),
				.pCode = reinterpret_cast<uint32_t const*>(particleVertFragBytes.data())
			};
			newShaderModule(particleVertFragCreate);

			std::vector<char> particleComputeBytes(Util::getFileBytes(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\particleCompute.spv)"));
			VkShaderModuleCreateInfo particleComputeCreate{
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = UINT32(particleComputeBytes.size()),
				.pCode = reinterpret_cast<uint32_t const*>(particleComputeBytes.data())
			};
			newShaderModule(particleComputeCreate);
		}

		VkShaderModule newShaderModule(VkShaderModuleCreateInfo const& CREATE) {
			VkShaderModule shaderModule{};

			VK_CHECK(vkCreateShaderModule(gDevice, &CREATE, nullptr, &shaderModule), "Failed to create shader module")
			gShaderModules.push_back(shaderModule);

			return shaderModule;
		}

		void clear() {
			for(VkShaderModule shaderModule : gShaderModules) {
				vkDestroyShaderModule(gDevice, shaderModule, nullptr);
			}
		}
	}
}