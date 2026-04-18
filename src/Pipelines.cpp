#include "Pipelines.h"
#include "PipelineLayouts.h"
#include "Util.h"
#include "LogicalDevice.h"
#include "ShaderModule.h"
#include "Vertex.hpp"
#include "Particle.hpp"

namespace Engine {
	namespace Pipelines {
		void add() {
			addGraphicsAggregates();
			addComputeCreates();
			createPipelines();
		}

		void addGraphicsAggregates() noexcept {
			gGraphicsAggregates.reserve(2);

			gGraphicsAggregates.emplace_back(
				std::vector<VkVertexInputBindingDescription>{ Vertex::Vertex::getInputBinding(0) },	
				Vertex::Vertex::getInputAttributes(0),
				std::vector<VkViewport>{ VkViewport{} },
				std::vector<VkRect2D>{ VkRect2D{} },
				VkSampleMask{~0U},
				std::vector<VkPipelineColorBlendAttachmentState>{
					VkPipelineColorBlendAttachmentState{
						.blendEnable = VK_FALSE,
						.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT 
					}
				},
				std::vector<VkDynamicState>{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR },
				std::vector<VkFormat>{ VK_FORMAT_R8G8B8A8_SRGB },
				std::vector<VkPipelineShaderStageCreateInfo>{
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
				VkPipelineRasterizationStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
					.depthClampEnable = VK_FALSE,
					.rasterizerDiscardEnable = VK_FALSE,
					.polygonMode = VK_POLYGON_MODE_FILL,
					.cullMode = VK_CULL_MODE_NONE,
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
				VkPipelineDynamicStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				},
				VkPipelineRenderingCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
					.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT,
				},
				PipelineLayouts::gLayouts[0]
			);

			gGraphicsAggregates.emplace_back(
				std::vector<VkVertexInputBindingDescription>{ Particle::Particle::getInputBinding(0) },	
				Particle::Particle::getInputAttributes(0),
				std::vector<VkViewport>{ VkViewport{} },
				std::vector<VkRect2D>{ VkRect2D{} },
				VkSampleMask{~0U},
				std::vector<VkPipelineColorBlendAttachmentState>{
					VkPipelineColorBlendAttachmentState{
						.blendEnable = VK_TRUE,
						.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
						.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
						.colorBlendOp = VK_BLEND_OP_ADD,
						.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
						.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
						.alphaBlendOp = VK_BLEND_OP_ADD,
						.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT 
					}
				},
				std::vector<VkDynamicState>{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR },
				std::vector<VkFormat>{ VK_FORMAT_R8G8B8A8_SRGB },
				std::vector<VkPipelineShaderStageCreateInfo>{
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_VERTEX_BIT,
						.module = ShaderModule::gShaderModules[1],
						.pName = "vertexShader"
					},
					VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
						.module = ShaderModule::gShaderModules[1],
						.pName = "fragmentShader"
					}
				},
				VkPipelineVertexInputStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
				},
				VkPipelineInputAssemblyStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
					.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
					.primitiveRestartEnable = VK_FALSE
				},
				VkPipelineTessellationStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO
				},
				VkPipelineViewportStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO
				},
				VkPipelineRasterizationStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
					.depthClampEnable = VK_FALSE,
					.rasterizerDiscardEnable = VK_FALSE,
					.polygonMode = VK_POLYGON_MODE_FILL,
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
				VkPipelineDepthStencilStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
					.depthTestEnable = VK_FALSE,
					.depthWriteEnable = VK_FALSE,
				},
				VkPipelineColorBlendStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
					.logicOpEnable = VK_FALSE,	
				},
				VkPipelineDynamicStateCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				},
				VkPipelineRenderingCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
				},
				PipelineLayouts::gLayouts[1]
			);
		}

		void addComputeCreates() noexcept {
			gComputeCreates.emplace_back(
				VkComputePipelineCreateInfo{
					.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
					.stage = VkPipelineShaderStageCreateInfo{
						.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage = VK_SHADER_STAGE_COMPUTE_BIT,
						.module = ShaderModule::gShaderModules[2],
						.pName = "computeShader"
					},
					.layout = PipelineLayouts::gLayouts[2]
				}
			);
		}

		void createPipelines() {
			for(GraphicsAggregate const& GRAPHICS_AGGREGATE : gGraphicsAggregates) {
				newGraphicsPipeline(GRAPHICS_AGGREGATE);
			}
			for(VkComputePipelineCreateInfo const& COMPUTE_CREATE : gComputeCreates) {
				newComputePipeline(COMPUTE_CREATE);
			}
		}

		VkPipeline newGraphicsPipeline(GraphicsAggregate const& AGGREGATE) {
			VkPipeline graphics{};

			CHECK_VK_SUCCESS(vkCreateGraphicsPipelines(gDevice, VK_NULL_HANDLE, 1, &AGGREGATE.create, nullptr, &graphics), "Failed to create graphics pipeline")
			gPipelines.push_back(graphics);

			return graphics;
		}

		VkPipeline newComputePipeline(VkComputePipelineCreateInfo const& CREATE) {
			VkPipeline compute{};

			CHECK_VK_SUCCESS(vkCreateComputePipelines(gDevice, VK_NULL_HANDLE, 1, &CREATE, nullptr, &compute), "Failed to create compute pipeline")
			gPipelines.push_back(compute);

			return compute;
		}

		void clear() noexcept {
			for(VkPipeline pipeline : gPipelines) {
				vkDestroyPipeline(gDevice, pipeline, nullptr);
			}
		}
	}
}