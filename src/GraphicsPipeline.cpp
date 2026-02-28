#include <iostream>
#include "GraphicsPipeline.hpp"
#include "Vertex.hpp"

namespace Engine {
	GraphicsPipeline::GraphicsPipeline(Backend::Devices* givenDevices, CreateInfo&& salvageCreateInfo) :
		devices{ givenDevices },
		graphicsPipeline{},
		createInfo(std::move(salvageCreateInfo)) {
		CHECK_VK_SUCCESS(
		vkCreateGraphicsPipelines(devices->getLogicalDevice(), nullptr, 1, &createInfo.graphicsPipeline, nullptr, &graphicsPipeline),
		"Failed to create graphics pipeline"
		)
	}

	GraphicsPipeline::~GraphicsPipeline() {
		vkDestroyShaderModule(devices->getLogicalDevice(), createInfo.stages[0].module, nullptr);
		vkDestroyPipelineLayout(devices->getLogicalDevice(), createInfo.graphicsPipeline.layout, nullptr);
		vkDestroyPipeline(devices->getLogicalDevice(), graphicsPipeline, nullptr);
	}
}