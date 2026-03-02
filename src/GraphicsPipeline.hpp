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
			CreateInfo(const VkGraphicsPipelineCreateInfo& GRAPHICS_PIPELINE, const VkPipelineRenderingCreateInfo& RENDERING, std::vector<VkFormat>&& salvageCAFormats, std::vector<VkPipelineShaderStageCreateInfo>&& salvageStages, VkPipelineVertexInputStateCreateInfo const& VERTEX_INPUT, std::vector<VkVertexInputBindingDescription>&& salvageVertexBindings, std::vector<VkVertexInputAttributeDescription>&& salvageVertexAttributes, VkPipelineInputAssemblyStateCreateInfo const& INPUT_ASSEMBLY, VkPipelineTessellationStateCreateInfo const& TESSELLATION, VkPipelineViewportStateCreateInfo const& VIEWPORT, VkPipelineRasterizationStateCreateInfo const& RASTERIZATION, VkPipelineMultisampleStateCreateInfo const& MULTISAMPLING, VkPipelineDepthStencilStateCreateInfo const& DEPTH_STENCIL, VkPipelineColorBlendStateCreateInfo const& COLOR_BLEND, std::vector<VkPipelineColorBlendAttachmentState>&& salvageColorBlendAttachments, VkPipelineDynamicStateCreateInfo const& DYNAMIC_STATE_INFO, std::vector<VkDynamicState>&& salvageDynamicStates) : 
				graphicsPipeline(GRAPHICS_PIPELINE), 
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