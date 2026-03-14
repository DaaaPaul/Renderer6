#include <iostream>
#include "GraphicsPipeline.hpp"

namespace Engine {
	GraphicsPipeline::GraphicsPipeline(Backend::Devices* pGivenDevices, CreateInfo&& salvageCreateInfo) :
		pDevices{ pGivenDevices },
		pGraphicsPipeline{},
		createInfo(std::move(salvageCreateInfo)) {
		CHECK_VK_SUCCESS(
		vkCreateGraphicsPipelines(pDevices->getLogicalDevice(), nullptr, 1, &createInfo.pipelineInfo, nullptr, &pGraphicsPipeline),
		"Failed to create graphics pipeline"
		)
	}

	GraphicsPipeline::~GraphicsPipeline() {
		vkDestroyPipeline(pDevices->getLogicalDevice(), pGraphicsPipeline, nullptr);
	}
}