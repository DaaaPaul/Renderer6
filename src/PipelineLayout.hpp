#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Util.h"
#include "Devices.h"

namespace Engine {
	class PipelineLayout {
		private:
		Backend::Devices* pDevices{};
		VkPipelineLayout pLayout{};
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
		std::vector<VkPushConstantRange> pushConstantRanges{};

		public:
		explicit PipelineLayout(Backend::Devices* givenDevices, std::vector<VkDescriptorSetLayout>&& salvageDescriptorSetLayouts, std::vector<VkPushConstantRange>&& salvagePushConstantRanges);
		~PipelineLayout();
		[[nodiscard]] VkPipelineLayout& getLayout() { return pLayout; }
		[[nodiscard]] std::vector<VkDescriptorSetLayout>& getDescriptorSetLayouts() { return descriptorSetLayouts; }
		[[nodiscard]] std::vector<VkPushConstantRange>& getPushConstantRanges() { return pushConstantRanges; }

		DELETE_COPY_CONSTRUCTORS(PipelineLayout)
		DELETE_MOVE_CONSTRUCTORS(PipelineLayout)
	};
}