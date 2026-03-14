#include "ShaderModule.hpp"

namespace Engine {
	ShaderModule::ShaderModule(Backend::Devices* givenDevices, std::vector<char>&& salvageCompiledBytes) :
		pDevices{ givenDevices },
		pShaderModule{},
		compiledBytes(std::move(salvageCompiledBytes)) {
		VkShaderModuleCreateInfo shaderModuleCreate{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = static_cast<uint32_t>(compiledBytes.size()),
			.pCode = reinterpret_cast<uint32_t const*>(compiledBytes.data())
		};
		CHECK_VK_SUCCESS(
			vkCreateShaderModule(pDevices->getLogicalDevice(), &shaderModuleCreate, nullptr, &pShaderModule),
			"Failed to create shader module"
		)
	}
	
	ShaderModule::~ShaderModule() {
		vkDestroyShaderModule(pDevices->getLogicalDevice(), pShaderModule, nullptr);
	}
}