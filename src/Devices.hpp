#pragma once

#include "Util.h"
#include "Instance.hpp"
#include "VChain.hpp"

namespace Backend {
	class Devices {
		private:
		Instance* pInstance{};
		VkPhysicalDevice pPhysicalDevice{};
		VkLogicalDevice pLogicalDevice{};
		std::vector<VkQueue> graphicsQueues{};
		const CreateInfo CREATE_INFO;
		const uint32_t GRAPHICS_QF_INDEX{ UINT32_MAX };

		public:		
		explicit Devices(Instance* pGivenInstance, CreateInfo&& givenCreateInfo);
		~Devices();
	};
}