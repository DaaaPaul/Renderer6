#include "PipelineLayout.hpp"

namespace Engine {
	PipelineLayout::PipelineLayout(Backend::Devices* givenDevices, std::vector<VkDescriptorSetLayout>&& salvageDescriptorSetLayouts) :
		pDevices{ givenDevices },
		descriptorSetLayouts(std::move(salvageDescriptorSetLayouts)) {
		VkPipelineLayoutCreateInfo pipelineLayoutCreate{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data()
		};
		CHECK_VK_SUCCESS(
			vkCreatePipelineLayout(pDevices->getLogicalDevice(), &pipelineLayoutCreate, nullptr, &pLayout),
			"Failed to create pipeline layout"
		)
	}
	
	PipelineLayout::~PipelineLayout() {
		vkDestroyPipelineLayout(pDevices->getLogicalDevice(), pLayout, nullptr);
	}
}