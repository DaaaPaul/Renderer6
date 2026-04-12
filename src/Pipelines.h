#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Util.h"

namespace Engine {
	namespace Pipelines {
		struct GraphicsAggregate {
			VkGraphicsPipelineCreateInfo create{};
			std::vector<VkPipelineShaderStageCreateInfo> shaders{};
			VkPipelineVertexInputStateCreateInfo vertexInput{}; // has indirection
			std::vector<VkVertexInputBindingDescription> vertexBindings{};
			std::vector<VkVertexInputAttributeDescription> vertexAttributes{};
			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			VkPipelineTessellationStateCreateInfo tessellation{};
			VkPipelineViewportStateCreateInfo viewport{}; // has indirection
			std::vector<VkViewport> viewports{};
			std::vector<VkRect2D> scissors{};
			VkPipelineRasterizationStateCreateInfo rasterization{};
			VkPipelineMultisampleStateCreateInfo multisample{}; // has indirection
			VkSampleMask sampleMask{};
			VkPipelineDepthStencilStateCreateInfo depthStencil{};
			VkPipelineColorBlendStateCreateInfo colorBlend{}; // has indirection
			std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
			VkPipelineDynamicStateCreateInfo dynamicState{}; // has indirection
			std::vector<VkDynamicState> dynamicStates{};
			VkPipelineRenderingCreateInfo rendering{}; // has indirection
			std::vector<VkFormat> colorAttachmentFormats{};
			VkPipelineLayout layout{};

			explicit GraphicsAggregate(std::vector<VkPipelineShaderStageCreateInfo> const& SHADERS, VkPipelineVertexInputStateCreateInfo const& VERTEX_INPUT, std::vector<VkVertexInputBindingDescription> const& VERTEX_BINDINGS, std::vector<VkVertexInputAttributeDescription> const& VERTEX_ATTRIBUTES, VkPipelineInputAssemblyStateCreateInfo const& INPUT_ASSEMBLY, VkPipelineTessellationStateCreateInfo const& TESSELLATION, VkPipelineViewportStateCreateInfo const& VIEWPORT, std::vector<VkViewport> const& VIEWPORTS, std::vector<VkRect2D> const& SCISSORS, VkPipelineRasterizationStateCreateInfo const& RASTERIZATION, VkPipelineMultisampleStateCreateInfo const& MULTISAMPLE, VkSampleMask const& SAMPLE_MASK, VkPipelineDepthStencilStateCreateInfo const& DEPTH_STENCIL, VkPipelineColorBlendStateCreateInfo const& COLOR_BLEND, std::vector<VkPipelineColorBlendAttachmentState> const& COLOR_BLEND_ATTACHMENTS, VkPipelineDynamicStateCreateInfo const& DYNAMIC_STATE, std::vector<VkDynamicState> const& DYNAMIC_STATES, VkPipelineRenderingCreateInfo const& RENDERING, std::vector<VkFormat> const& COLOR_ATTACHMENT_FORMATS, VkPipelineLayout const& LAYOUT) :
				vertexBindings{ VERTEX_BINDINGS }, vertexAttributes{ VERTEX_ATTRIBUTES }, viewports{ VIEWPORTS }, scissors{ SCISSORS }, sampleMask{ SAMPLE_MASK }, colorBlendAttachments{ COLOR_BLEND_ATTACHMENTS }, dynamicStates{ DYNAMIC_STATES }, colorAttachmentFormats{ COLOR_ATTACHMENT_FORMATS },
				shaders{ SHADERS }, vertexInput{ VERTEX_INPUT }, inputAssembly{ INPUT_ASSEMBLY }, tessellation{ TESSELLATION }, viewport{ VIEWPORT }, rasterization{ RASTERIZATION }, multisample{ MULTISAMPLE }, depthStencil{ DEPTH_STENCIL }, colorBlend{ COLOR_BLEND }, dynamicState{ DYNAMIC_STATE }, rendering{ RENDERING }, layout{ LAYOUT }, 
				create{ .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, .pNext = &rendering, .stageCount = UINT32(shaders.size()), .pStages = shaders.data(), .pVertexInputState = &vertexInput, .pInputAssemblyState = &inputAssembly, .pTessellationState = &tessellation, .pViewportState = &viewport, .pRasterizationState = &rasterization, .pMultisampleState = &multisample, .pDepthStencilState = &depthStencil, .pColorBlendState = &colorBlend, .pDynamicState = &dynamicState, .layout = layout } {
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

		void addGraphicsAggregates() noexcept;
		void addComputeCreates() noexcept;
		void createPipelines();

		VkPipeline newGraphicsPipeline(VkGraphicsPipelineCreateInfo const&);
		VkPipeline newComputePipeline(VkComputePipelineCreateInfo const&);
		void clear() noexcept;
	}
}