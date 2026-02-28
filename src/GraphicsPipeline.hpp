#pragma once

#include "Devices.hpp"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "Common.h"

namespace Engine {
	class GraphicsPipeline {
		public:
		struct CreateInfo {
			VkGraphicsPipelineCreateInfo graphicsPipeline{};

			VkPipelineRenderingCreateInfo rendering{};
			std::vector<VkFormat> colorAttachmentFormats{};

			std::vector<VkPipelineShaderStageCreateInfo> stages{};
			//VkShaderModule shaderModules{};
			//VkShaderModuleCreateInfo shaderModuleInfos{};
			//std::vector<char> sprivFileBytes{};

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
			//VkPipelineLayout layout{};
			//VkPipelineLayoutCreateInfo layoutInfo{};
			//std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};

			void reroutePointers() {
				graphicsPipeline.pNext = &rendering;
				graphicsPipeline.pVertexInputState = &vertexInput;
				graphicsPipeline.pInputAssemblyState = &inputAssembly;
				graphicsPipeline.pTessellationState = &tesselation;
				graphicsPipeline.pViewportState = &viewport;
				graphicsPipeline.pRasterizationState = &rasterization;
				graphicsPipeline.pMultisampleState = &multisampling;
				graphicsPipeline.pDepthStencilState = &depthStencil;
				graphicsPipeline.pColorBlendState = &colorBlend;
				graphicsPipeline.pDynamicState = &dynamicStateInfo;
			}
			CreateInfo(const VkGraphicsPipelineCreateInfo& GRAPHICS_PIPELINE, const VkPipelineRenderingCreateInfo& RENDERING, const std::vector<VkFormat>& COLOR_ATTACHMENT_FORMATS, const std::vector<VkPipelineShaderStageCreateInfo>& STAGES, const VkPipelineVertexInputStateCreateInfo& VERTEX_INPUT, const std::vector<VkVertexInputBindingDescription>& VERTEX_BINDINGS, const std::vector<VkVertexInputAttributeDescription>& VERTEX_ATTRIBUTES, const VkPipelineInputAssemblyStateCreateInfo& INPUT_ASSEMBLY, const VkPipelineTessellationStateCreateInfo& TESSELLATION, const VkPipelineViewportStateCreateInfo& VIEWPORT, const VkPipelineRasterizationStateCreateInfo& RASTERIZATION, const VkPipelineMultisampleStateCreateInfo& MULTISAMPLING, const VkPipelineDepthStencilStateCreateInfo& DEPTH_STENCIL, const VkPipelineColorBlendStateCreateInfo& COLOR_BLEND, const std::vector<VkPipelineColorBlendAttachmentState>& COLOR_BLEND_ATTACHMENTS, const VkPipelineDynamicStateCreateInfo& DYNAMIC_STATE_INFO, const std::vector<VkDynamicState>& DYNAMIC_STATES) : 
				graphicsPipeline(GRAPHICS_PIPELINE), 
				rendering(RENDERING), 
				colorAttachmentFormats(COLOR_ATTACHMENT_FORMATS), 
				stages(STAGES), 
				vertexInput(VERTEX_INPUT), 
				vertexBindings(VERTEX_BINDINGS), 
				vertexAttributes(VERTEX_ATTRIBUTES), 
				inputAssembly(INPUT_ASSEMBLY), 
				tesselation(TESSELLATION), 
				viewport(VIEWPORT), 
				rasterization(RASTERIZATION), 
				multisampling(MULTISAMPLING), 
				depthStencil(DEPTH_STENCIL), 
				colorBlend(COLOR_BLEND), 
				colorBlendAttachments(COLOR_BLEND_ATTACHMENTS), 
				dynamicStateInfo(DYNAMIC_STATE_INFO), 
				dynamicStates(DYNAMIC_STATES) {
					reroutePointers();
			}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				graphicsPipeline(salvageCreateInfo.graphicsPipeline), 
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
			DELETE_COPY_CONSTRUCTORS(CreateInfo)
		};

		private:
		Backend::Devices* devices{};
		VkPipeline graphicsPipeline{};
		CreateInfo createInfo;

		public:
		explicit GraphicsPipeline(Backend::Devices* givenDevices, CreateInfo&& salvageCreateInfo);
		~GraphicsPipeline();
		[[nodiscard]] Backend::Devices*& getDevices() { return devices; }
		[[nodiscard]] VkPipeline& getGraphicsPipeline() { return graphicsPipeline; }
		[[nodiscard]] CreateInfo& getCreateInfo() { return createInfo; }

		DELETE_COPY_CONSTRUCTORS(GraphicsPipeline)
		DELETE_MOVE_CONSTRUCTORS(GraphicsPipeline)
	};
}