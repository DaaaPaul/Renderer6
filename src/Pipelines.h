#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include "Util.h"

namespace Engine {
	namespace Pipelines {
		struct GraphicsAggregate {
			std::vector<VkVertexInputBindingDescription> vertexBindings{};
			std::vector<VkVertexInputAttributeDescription> vertexAttributes{};
			std::vector<VkViewport> viewports{};
			std::vector<VkRect2D> scissors{};
			VkSampleMask sampleMask{};
			std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
			std::vector<VkDynamicState> dynamicStates{};
			std::vector<VkFormat> colorAttachmentFormats{};

			std::vector<VkPipelineShaderStageCreateInfo> shaders{};
			VkPipelineVertexInputStateCreateInfo vertexInput{};
			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			VkPipelineTessellationStateCreateInfo tessellation{};
			VkPipelineViewportStateCreateInfo viewport{};
			VkPipelineRasterizationStateCreateInfo rasterization{};
			VkPipelineMultisampleStateCreateInfo multisample{};
			VkPipelineDepthStencilStateCreateInfo depthStencil{};
			VkPipelineColorBlendStateCreateInfo colorBlend{};
			VkPipelineDynamicStateCreateInfo dynamicState{};
			VkPipelineRenderingCreateInfo rendering{};
			VkPipelineLayout layout{};

			VkGraphicsPipelineCreateInfo create{};

			explicit GraphicsAggregate(std::vector<VkVertexInputBindingDescription> const& VERTEX_BINDINGS, std::vector<VkVertexInputAttributeDescription> const& VERTEX_ATTRIBUTES, std::vector<VkViewport> const& VIEWPORTS, std::vector<VkRect2D> const& SCISSORS, VkSampleMask const& SAMPLE_MASK, std::vector<VkPipelineColorBlendAttachmentState> const& COLOR_BLEND_ATTACHMENTS, std::vector<VkDynamicState> const& DYNAMIC_STATES, std::vector<VkFormat> const& COLOR_ATTACHMENT_FORMATS, std::vector<VkPipelineShaderStageCreateInfo> const& SHADERS, VkPipelineVertexInputStateCreateInfo const& VERTEX_INPUT, VkPipelineInputAssemblyStateCreateInfo const& INPUT_ASSEMBLY, VkPipelineTessellationStateCreateInfo const& TESSELLATION, VkPipelineViewportStateCreateInfo const& VIEWPORT, VkPipelineRasterizationStateCreateInfo const& RASTERIZATION, VkPipelineMultisampleStateCreateInfo const& MULTISAMPLE, VkPipelineDepthStencilStateCreateInfo const& DEPTH_STENCIL, VkPipelineColorBlendStateCreateInfo const& COLOR_BLEND, VkPipelineDynamicStateCreateInfo const& DYNAMIC_STATE, VkPipelineRenderingCreateInfo const& RENDERING, VkPipelineLayout const& LAYOUT) :
				vertexBindings{ VERTEX_BINDINGS }, vertexAttributes{ VERTEX_ATTRIBUTES }, viewports{ VIEWPORTS }, scissors{ SCISSORS }, sampleMask{ SAMPLE_MASK }, colorBlendAttachments{ COLOR_BLEND_ATTACHMENTS }, dynamicStates{ DYNAMIC_STATES }, colorAttachmentFormats{ COLOR_ATTACHMENT_FORMATS },
				shaders{ SHADERS }, vertexInput{ VERTEX_INPUT }, inputAssembly{ INPUT_ASSEMBLY }, tessellation{ TESSELLATION }, viewport{ VIEWPORT }, rasterization{ RASTERIZATION }, multisample{ MULTISAMPLE }, depthStencil{ DEPTH_STENCIL }, colorBlend{ COLOR_BLEND }, dynamicState{ DYNAMIC_STATE }, rendering{ RENDERING }, layout{ LAYOUT }, 
				create{ .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, .pNext = &rendering, .stageCount = UINT32(shaders.size()), .pStages = shaders.data(), .pVertexInputState = &vertexInput, .pInputAssemblyState = &inputAssembly, .pTessellationState = &tessellation, .pViewportState = &viewport, .pRasterizationState = &rasterization, .pMultisampleState = &multisample, .pDepthStencilState = &depthStencil, .pColorBlendState = &colorBlend, .pDynamicState = &dynamicState, .layout = layout } {
				selfRefer();
			}

			private:
			void selfRefer() {
				vertexInput.vertexBindingDescriptionCount = UINT32(vertexBindings.size());
				vertexInput.pVertexBindingDescriptions = vertexBindings.data();
				vertexInput.vertexAttributeDescriptionCount = UINT32(vertexAttributes.size());
				vertexInput.pVertexAttributeDescriptions = vertexAttributes.data();

				viewport.viewportCount = UINT32(viewports.size());
				viewport.pViewports = viewports.data();
				viewport.scissorCount = UINT32(scissors.size());
				viewport.pScissors = scissors.data();

				multisample.pSampleMask = &sampleMask;

				colorBlend.attachmentCount = UINT32(colorBlendAttachments.size());
				colorBlend.pAttachments = colorBlendAttachments.data();

				dynamicState.dynamicStateCount = UINT32(dynamicStates.size());
				dynamicState.pDynamicStates = dynamicStates.data();

				rendering.colorAttachmentCount = UINT32(colorAttachmentFormats.size());
				rendering.pColorAttachmentFormats = colorAttachmentFormats.data();
			}
		};

		inline std::vector<VkPipeline> gPipelines{};
		inline std::vector<GraphicsAggregate> gGraphicsAggregates{};
		inline std::vector<VkComputePipelineCreateInfo> gComputeCreates{};

		void add();

		void addGraphicsAggregates();
		void addComputeCreates();
		void createPipelines();

		VkPipeline newGraphicsPipeline(GraphicsAggregate const&);
		VkPipeline newComputePipeline(VkComputePipelineCreateInfo const&);
		void clear();
	}
}