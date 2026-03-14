#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Common.h"
#include "Devices.hpp"

namespace Engine {
	class PipelineLayout {
		private:
		Backend::Devices* pDevices{};
		VkPipelineLayout pLayout{};
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};

		public:
		explicit PipelineLayout(Backend::Devices* givenDevices, std::vector<VkDescriptorSetLayout>&& salvageDescriptorSetLayouts);
		~PipelineLayout();
		[[nodiscard]] VkPipelineLayout& getLayout() { return pLayout; }
		[[nodiscard]] std::vector<VkDescriptorSetLayout>& getDescriptorSetLayouts() { return descriptorSetLayouts; }

		DELETE_COPY_CONSTRUCTORS(PipelineLayout)
		DELETE_MOVE_CONSTRUCTORS(PipelineLayout)
	};
}