#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Common.h"
#include "Devices.hpp"

namespace Engine {
	class ShaderModule {
		private:
		Backend::Devices* pDevices{};
		VkShaderModule pShaderModule{};
		std::vector<char> compiledBytes{};

		public:
		explicit ShaderModule(Backend::Devices* givenDevices, std::vector<char>&& salvageCompiledBytes);
		~ShaderModule();
		[[nodiscard]] VkShaderModule& getShaderModule() { return pShaderModule; }
		[[nodiscard]] std::vector<char>& getCompiledBytes() { return compiledBytes; }

		DELETE_COPY_CONSTRUCTORS(ShaderModule)
		DELETE_MOVE_CONSTRUCTORS(ShaderModule)
	};
}