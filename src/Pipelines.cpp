#include "Pipelines.h"
#include "PipelineLayouts.h"
#include "Util.h"
#include "LogicalDevice.h"
#include "ShaderModule.h"
#include "Vertex.hpp"

namespace Engine {
	namespace Pipelines {
		void add() {
			addGraphicsAggregates();
			addComputeAggregates();
			createPipelines();
		}

		void addGraphicsAggregates() {
			gGraphicsAggregates.emplace_back(
				{
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_VERTEX_BIT,
						.module = ShaderModule::gShaderModules[0],
						.pName = "vertexShader"
					},
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
						.module = ShaderModule::gShaderModules[0],
						.pName = "fragmentShader"
					}
				},
				VkPipelineVertexInputStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
				},
				{ Vertex::Vertex::getInputBinding(0) },
				Vertex::Vertex::getInputAttributes(0),
				VkPipelineInputAssemblyStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
					.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
					.primitiveRestartEnable = VK_FALSE
				},
				VkPipelineTessellationStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO
				},
				VkPipelineViewportStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO
				},
				{ VkViewport{} },
				{ VkRect2D{} },
				VkPipelineRasterizationStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
					.depthClampEnable = VK_FALSE,
					.rasterizerDiscardEnable = VK_FALSE,
					.polygonMode = VK_POLYGON_MODE_FILL,
					.cullMode = VK_CULL_MODE_BACK_BIT,
					.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
					.depthBiasEnable = VK_FALSE,
					.depthBiasConstantFactor = 0.0f,
					.depthBiasClamp = 0.0f,
					.depthBiasSlopeFactor = 1.0f,
					.lineWidth = 1.0f
				},
				VkPipelineMultisampleStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
					.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
					.sampleShadingEnable = VK_FALSE,
					.alphaToCoverageEnable = VK_FALSE,
					.alphaToOneEnable = VK_FALSE,
				},
				VkSampleMask{},
				VkPipelineDepthStencilStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
					.depthTestEnable = VK_TRUE,
					.depthWriteEnable = VK_TRUE,
					.depthCompareOp = VK_COMPARE_OP_LESS,
					.depthBoundsTestEnable = VK_FALSE,
					.stencilTestEnable = VK_FALSE,
				},
				VkPipelineColorBlendStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
					.logicOpEnable = VK_FALSE,	
				},
				{
					VkPipelineColorBlendAttachmentState{
						.blendEnable = VK_FALSE,
						.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT 
					}
				},
				VkPipelineDynamicStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				},
				std::vector<VkDynamicState>{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR },
				VkPipelineRenderingCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
					.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT,
				},
				std::vector<VkFormat>{ VK_FORMAT_R8G8B8A8_SRGB },
				PipelineLayouts::gLayouts[0]
			);
		}

		void addComputeAggregates() {

		}

		void createPipelines() {

		}

		VkPipeline newGraphicsPipeline(VkGraphicsPipelineCreateInfo const& CREATE) {
			VkPipeline graphics{};

			CHECK_VK_SUCCESS(vkCreateGraphicsPipelines(gpDevice, VK_NULL_HANDLE, 1, &CREATE, nullptr, &graphics), "Failed to create graphics pipeline")
			gPipelines.push_back(graphics);

			return graphics;
		}

		VkPipeline newComputePipeline(VkComputePipelineCreateInfo const& CREATE) {
			VkPipeline compute{};

			CHECK_VK_SUCCESS(vkCreateComputePipelines(gpDevice, VK_NULL_HANDLE, 1, &CREATE, nullptr, &compute), "Failed to create compute pipeline")
			gPipelines.push_back(compute);

			return compute;
		}

		void clear() noexcept {
			for(VkPipeline& pipeline : gPipelines) {
				vkDestroyPipeline(gpDevice, pipeline, nullptr);
			}
		}
	}
}