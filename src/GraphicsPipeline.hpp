#pragma once

#include "Devices.hpp"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "Common.h"

namespace Engine {
	class GraphicsPipeline {
		public:
		struct CreateInfo {
			VkGraphicsPipelineCreateInfo pGraphicsPipeline{};

			VkPipelineRenderingCreateInfo rendering{};
			std::vector<VkFormat> colorAttachmentFormats{};

			std::vector<VkPipelineShaderStageCreateInfo> stages{};

			VkPipelineVertexInputStateCreateInfo vertexInput{};
			std::vector<VkVertexInputBindingDescription> vertexBindings{};
			std::vector<VkVertexInputAttributeDescription> vertexAttributes{};

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

			VkPipelineTessellationStateCreateInfo tesselation{};

			VkPipelineViewportStateCreateInfo viewport{};

			VkPipelineRasterizationStateCreateInfo rasterization{};

			VkPipelineMultisampleStateCreateInfo multisampling{};

			VkPipelineDepthStencilStateCreateInfo depthStencil{};

			VkPipelineColorBlendStateCreateInfo colorBlend{};
			std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};

			VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
			std::vector<VkDynamicState> dynamicStates{};

			void reroutePointers() {
				pGraphicsPipeline.pNext = &rendering;
				pGraphicsPipeline.pVertexInputState = &vertexInput;
				pGraphicsPipeline.pInputAssemblyState = &inputAssembly;
				pGraphicsPipeline.pTessellationState = &tesselation;
				pGraphicsPipeline.pViewportState = &viewport;
				pGraphicsPipeline.pRasterizationState = &rasterization;
				pGraphicsPipeline.pMultisampleState = &multisampling;
				pGraphicsPipeline.pDepthStencilState = &depthStencil;
				pGraphicsPipeline.pColorBlendState = &colorBlend;
				pGraphicsPipeline.pDynamicState = &dynamicStateInfo;
			}
			CreateInfo(const VkGraphicsPipelineCreateInfo& GRAPHICS_PIPELINE, const VkPipelineRenderingCreateInfo& RENDERING, std::vector<VkFormat>&& salvageCAFormats, std::vector<VkPipelineShaderStageCreateInfo>&& salvageStages, VkPipelineVertexInputStateCreateInfo const& VERTEX_INPUT, std::vector<VkVertexInputBindingDescription>&& salvageVertexBindings, std::vector<VkVertexInputAttributeDescription>&& salvageVertexAttributes, VkPipelineInputAssemblyStateCreateInfo const& INPUT_ASSEMBLY, VkPipelineTessellationStateCreateInfo const& TESSELLATION, VkPipelineViewportStateCreateInfo const& VIEWPORT, VkPipelineRasterizationStateCreateInfo const& RASTERIZATION, VkPipelineMultisampleStateCreateInfo const& MULTISAMPLING, VkPipelineDepthStencilStateCreateInfo const& DEPTH_STENCIL, VkPipelineColorBlendStateCreateInfo const& COLOR_BLEND, std::vector<VkPipelineColorBlendAttachmentState>&& salvageColorBlendAttachments, VkPipelineDynamicStateCreateInfo const& DYNAMIC_STATE_INFO, std::vector<VkDynamicState>&& salvageDynamicStates) : 
				pGraphicsPipeline(GRAPHICS_PIPELINE), 
				rendering(RENDERING), 
				colorAttachmentFormats(std::move(salvageCAFormats)),
				stages(std::move(salvageStages)),
				vertexInput(VERTEX_INPUT), 
				vertexBindings(std::move(salvageVertexBindings)),
				vertexAttributes(std::move(salvageVertexAttributes)),
				inputAssembly(INPUT_ASSEMBLY), 
				tesselation(TESSELLATION), 
				viewport(VIEWPORT), 
				rasterization(RASTERIZATION), 
				multisampling(MULTISAMPLING), 
				depthStencil(DEPTH_STENCIL), 
				colorBlend(COLOR_BLEND), 
				colorBlendAttachments(std::move(salvageColorBlendAttachments)),
				dynamicStateInfo(DYNAMIC_STATE_INFO), 
				dynamicStates(std::move(salvageDynamicStates)) {
					reroutePointers();
			}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				pGraphicsPipeline(salvageCreateInfo.pGraphicsPipeline), 
				rendering(salvageCreateInfo.rendering), 
				colorAttachmentFormats(std::move(salvageCreateInfo.colorAttachmentFormats)), 
				stages(std::move(salvageCreateInfo.stages)), 
				vertexInput(salvageCreateInfo.vertexInput), 
				vertexBindings(std::move(salvageCreateInfo.vertexBindings)), 
				vertexAttributes(std::move(salvageCreateInfo.vertexAttributes)), 
				inputAssembly(salvageCreateInfo.inputAssembly), 
				tesselation(salvageCreateInfo.tesselation), 
				viewport(salvageCreateInfo.viewport), 
				rasterization(salvageCreateInfo.rasterization), 
				multisampling(salvageCreateInfo.multisampling), 
				depthStencil(salvageCreateInfo.depthStencil), 
				colorBlend(salvageCreateInfo.colorBlend), 
				colorBlendAttachments(std::move(salvageCreateInfo.colorBlendAttachments)), 
				dynamicStateInfo(salvageCreateInfo.dynamicStateInfo), 
				dynamicStates(std::move(salvageCreateInfo.dynamicStates)) {
					reroutePointers();
			}
		};

		private:
		Backend::Devices* pDevices{};
		VkPipeline pGraphicsPipeline{};
		CreateInfo createInfo;

		public:
		explicit GraphicsPipeline(Backend::Devices* givenDevices, CreateInfo&& salvageCreateInfo);
		~GraphicsPipeline();
		[[nodiscard]] Backend::Devices*& getDevices() { return pDevices; }
		[[nodiscard]] VkPipeline& getGraphicsPipeline() { return pGraphicsPipeline; }
		[[nodiscard]] CreateInfo& getCreateInfo() { return createInfo; }

		DELETE_COPY_CONSTRUCTORS(GraphicsPipeline)
		DELETE_MOVE_CONSTRUCTORS(GraphicsPipeline)
	};
}