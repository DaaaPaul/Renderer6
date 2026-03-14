#include "ComputePipeline.hpp"

namespace Engine {
	ComputePipeline::ComputePipeline(Backend::Devices* pDevices, CreateInfo&& givenCreateInfo) : 
		pDevices(pDevices),
		pComputePipeline{},
		CREATE_INFO(std::move(givenCreateInfo)) {
		CHECK_VK_SUCCESS(
			vkCreateComputePipelines(pDevices->getLogicalDevice(), VK_NULL_HANDLE, 1, &CREATE_INFO.pipelineInfo, nullptr, &pComputePipeline),
			"Failed to create compute pipeline"
		)
	}

	ComputePipeline::~ComputePipeline() {
		vkDestroyShaderModule(pDevices->getLogicalDevice(), CREATE_INFO.pipelineInfo.stage.module, nullptr);
		vkDestroyPipelineLayout(pDevices->getLogicalDevice(), CREATE_INFO.pipelineInfo.layout, nullptr);
		vkDestroyPipeline(pDevices->getLogicalDevice(), pComputePipeline, nullptr);
	}
}