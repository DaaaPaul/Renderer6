#include "PipelineLayouts.h"
#include "LogicalDevice.h"
#include "MemoryDevice.h"

namespace PipelineLayouts {
	void add() {
		VkPushConstantRange vertexDataPointer{
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.offset = 0,
			.size = POINTER_SIZE(1)
		};
		VkPipelineLayoutCreateInfo modelPipelineLayoutCreate{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &Memory::Device::gDescriptorSets[0].layout, // texture image and sampler
			.pushConstantRangeCount = 1,
			.pPushConstantRanges = &vertexDataPointer
		};
		newLayout(modelPipelineLayoutCreate);

		// empty layout for particle graphics pipeline
		newLayout(VkPipelineLayoutCreateInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO });

		VkPushConstantRange computePointers{
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
			.offset = 0,
			.size = POINTER_SIZE(3)
		};
		VkPipelineLayoutCreateInfo computePipelineLayoutCreate{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pushConstantRangeCount = 1,
			.pPushConstantRanges = &computePointers
		};
		newLayout(computePipelineLayoutCreate);
	}

	VkPipelineLayout newLayout(VkPipelineLayoutCreateInfo const& CREATE) {
		VkPipelineLayout layout{};

		VK_CHECK(vkCreatePipelineLayout(gDevice, &CREATE, nullptr, &layout), "Failed to create pipeline layout")
		gLayouts.push_back(layout);

		return layout;
	}

	void clear() {
		for(VkPipelineLayout layout : gLayouts) {
			vkDestroyPipelineLayout(gDevice, layout, nullptr);
		}
	}
}
