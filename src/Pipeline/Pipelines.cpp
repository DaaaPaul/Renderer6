#include "Pipelines.h"
#include "PipelineLayouts.h"
#include "Backend/LogicalDevice.h"
#include "ShaderModule.h"
#include "Backend/Swapchain.h"
#include "Utility/Vulkan.h"
#include "ShaderStructs/PBRVertex.hpp"

namespace Pipelines {
	void init() {
		{
			VkPipeline pbr_pipeline{};

			std::vector<VkFormat> color_attachment_formats{ Swapchain::g_IMAGE_FORMAT };
			VkPipelineRenderingCreateInfo rendering_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
				.colorAttachmentCount = static_cast<uint32_t>(color_attachment_formats.size()),
				.pColorAttachmentFormats = color_attachment_formats.data(),
				.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT
			};

			std::vector<VkPipelineShaderStageCreateInfo> shader_stages{
				VkPipelineShaderStageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_VERTEX_BIT,
					.module = ShaderModule::g_shader_modules[0],
					.pName = "vertex_main"
				},
				VkPipelineShaderStageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
					.module = ShaderModule::g_shader_modules[0],
					.pName = "fragment_main"
				}
			};

			std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions{ PBRVertex::get_vertex_input_binding_description(0) };
			std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions{ PBRVertex::get_vertex_input_attributes(0) };
			VkPipelineVertexInputStateCreateInfo vertex_input_state{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_binding_descriptions.size()),
				.pVertexBindingDescriptions = vertex_binding_descriptions.data(),
				.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attribute_descriptions.size()),
				.pVertexAttributeDescriptions = vertex_attribute_descriptions.data()
			};

			VkPipelineInputAssemblyStateCreateInfo input_assembly{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
			};

			VkPipelineTessellationStateCreateInfo tessellation{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO
			};

			VkPipelineViewportStateCreateInfo empty_viewport{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				.viewportCount = 1,
				.scissorCount = 1
			};

			VkPipelineRasterizationStateCreateInfo rasterization{
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
			};

			VkPipelineMultisampleStateCreateInfo multisample{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
				.sampleShadingEnable = VK_FALSE,
				.alphaToCoverageEnable = VK_FALSE,
				.alphaToOneEnable = VK_FALSE,
			};

			VkPipelineDepthStencilStateCreateInfo depth{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
				.depthTestEnable = VK_TRUE,
				.depthWriteEnable = VK_TRUE,
				.depthCompareOp = VK_COMPARE_OP_LESS,
				.depthBoundsTestEnable = VK_FALSE,
				.stencilTestEnable = VK_FALSE,
			};

			std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states{
				VkPipelineColorBlendAttachmentState{
					.blendEnable = VK_TRUE,
					.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
					.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
					.colorBlendOp = VK_BLEND_OP_ADD,
					.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
					.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
					.alphaBlendOp = VK_BLEND_OP_ADD,
					.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
				}
			};
			VkPipelineColorBlendStateCreateInfo color_blend{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE,
				.attachmentCount = static_cast<uint32_t>(color_blend_attachment_states.size()),
				.pAttachments = color_blend_attachment_states.data()
			};

			std::vector<VkDynamicState> dynamic_state_flags{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
			VkPipelineDynamicStateCreateInfo dynamic_state{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				.dynamicStateCount = static_cast<uint32_t>(dynamic_state_flags.size()),
				.pDynamicStates = dynamic_state_flags.data()
			};

			VkGraphicsPipelineCreateInfo create{
				.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				.pNext = &rendering_info,
				.stageCount = static_cast<uint32_t>(shader_stages.size()),
				.pStages = shader_stages.data(),
				.pVertexInputState = &vertex_input_state,
				.pInputAssemblyState = &input_assembly,
				.pTessellationState = &tessellation,
				.pViewportState = &empty_viewport,
				.pRasterizationState = &rasterization,
				.pMultisampleState = &multisample,
				.pDepthStencilState = &depth,
				.pColorBlendState = &color_blend,
				.pDynamicState = &dynamic_state,
				.layout = PipelineLayouts::g_layouts[0],
			};

			Vulkan::check(vkCreateGraphicsPipelines(g_device, VK_NULL_HANDLE, 1, &create, nullptr, &pbr_pipeline), "Pipelines::init: failed");

			g_pipelines.push_back(pbr_pipeline);
		}

		{
			VkPipeline simple_pipeline{};

			std::vector<VkFormat> color_attachment_formats{ Swapchain::g_IMAGE_FORMAT };
			VkPipelineRenderingCreateInfo rendering_info{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
				.colorAttachmentCount = static_cast<uint32_t>(color_attachment_formats.size()),
				.pColorAttachmentFormats = color_attachment_formats.data(),
				.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT
			};

			std::vector<VkPipelineShaderStageCreateInfo> shader_stages{
				VkPipelineShaderStageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_VERTEX_BIT,
					.module = ShaderModule::g_shader_modules[1],
					.pName = "vertex_main"
				},
				VkPipelineShaderStageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
					.module = ShaderModule::g_shader_modules[1],
					.pName = "fragment_main"
				}
			};

			std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions{ 
				VkVertexInputBindingDescription{
					.binding = 0,
					.stride = sizeof(glm::vec3),
					.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
				}
			};
			std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions{ 
				VkVertexInputAttributeDescription{
					.location = 0,
					.binding = 0,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = 0
				}
			};
			VkPipelineVertexInputStateCreateInfo vertex_input_state{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_binding_descriptions.size()),
				.pVertexBindingDescriptions = vertex_binding_descriptions.data(),
				.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attribute_descriptions.size()),
				.pVertexAttributeDescriptions = vertex_attribute_descriptions.data()
			};

			VkPipelineInputAssemblyStateCreateInfo input_assembly{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
			};

			VkPipelineTessellationStateCreateInfo tessellation{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO
			};

			VkPipelineViewportStateCreateInfo empty_viewport{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				.viewportCount = 1,
				.scissorCount = 1
			};

			VkPipelineRasterizationStateCreateInfo rasterization{
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
			};

			VkPipelineMultisampleStateCreateInfo multisample{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
				.sampleShadingEnable = VK_FALSE,
				.alphaToCoverageEnable = VK_FALSE,
				.alphaToOneEnable = VK_FALSE,
			};

			VkPipelineDepthStencilStateCreateInfo depth{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
				.depthTestEnable = VK_TRUE,
				.depthWriteEnable = VK_TRUE,
				.depthCompareOp = VK_COMPARE_OP_LESS,
				.depthBoundsTestEnable = VK_FALSE,
				.stencilTestEnable = VK_FALSE,
			};

			std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states{
				VkPipelineColorBlendAttachmentState{
					.blendEnable = VK_TRUE,
					.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
					.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
					.colorBlendOp = VK_BLEND_OP_ADD,
					.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
					.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
					.alphaBlendOp = VK_BLEND_OP_ADD,
					.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
				}
			};
			VkPipelineColorBlendStateCreateInfo color_blend{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE,
				.attachmentCount = static_cast<uint32_t>(color_blend_attachment_states.size()),
				.pAttachments = color_blend_attachment_states.data()
			};

			std::vector<VkDynamicState> dynamic_state_flags{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
			VkPipelineDynamicStateCreateInfo dynamic_state{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				.dynamicStateCount = static_cast<uint32_t>(dynamic_state_flags.size()),
				.pDynamicStates = dynamic_state_flags.data()
			};

			VkGraphicsPipelineCreateInfo create{
				.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				.pNext = &rendering_info,
				.stageCount = static_cast<uint32_t>(shader_stages.size()),
				.pStages = shader_stages.data(),
				.pVertexInputState = &vertex_input_state,
				.pInputAssemblyState = &input_assembly,
				.pTessellationState = &tessellation,
				.pViewportState = &empty_viewport,
				.pRasterizationState = &rasterization,
				.pMultisampleState = &multisample,
				.pDepthStencilState = &depth,
				.pColorBlendState = &color_blend,
				.pDynamicState = &dynamic_state,
				.layout = PipelineLayouts::g_layouts[1],
			};

			Vulkan::check(vkCreateGraphicsPipelines(g_device, VK_NULL_HANDLE, 1, &create, nullptr, &simple_pipeline), "Pipelines::init: failed");

			g_pipelines.push_back(simple_pipeline);
		}
	}

	void clear() {
		for(VkPipeline pipeline : g_pipelines) {
			vkDestroyPipeline(g_device, pipeline, nullptr);
		}
		g_pipelines.clear();
	}
}
