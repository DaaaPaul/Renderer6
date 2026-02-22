#include <iostream>
#include "GraphicsPipeline.hpp"
#include "Vertex.hpp"

namespace Engine {
	GraphicsPipeline::GraphicsPipeline() :
		devices{},
		mpGraphicsPipeline{},
		CREATE_INFO{} {}

	GraphicsPipeline::GraphicsPipeline(Backend::Devices* givenDevices, GraphicsPipelineConstructInfo const& GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO) :
		devices{ givenDevices },
		mpGraphicsPipeline{},
		CREATE_INFO{ GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO } {

		// reroute pointers
		{
			CREATE_INFO.mRendering.colorAttachmentCount = static_cast<uint32_t>(CREATE_INFO.mCOLOR_ATTACHMENT_FORMATS.size());
			CREATE_INFO.mRendering.pColorAttachmentFormats = CREATE_INFO.mCOLOR_ATTACHMENT_FORMATS.data();

			CREATE_INFO.mShaderModuleCreateInfo.codeSize = static_cast<uint32_t>(CREATE_INFO.mSHADERS_SPRIV_FILE_BYTES.size());
			CREATE_INFO.mShaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t const*>(CREATE_INFO.mSHADERS_SPRIV_FILE_BYTES.data());
			CHECK_VK_SUCCESS(
			vkCreateShaderModule(devices->getLogicalDevice(), &CREATE_INFO.mShaderModuleCreateInfo, nullptr, &CREATE_INFO.mpShaderModule),
			"Failed to create shader module"
			)
			for(VkPipelineShaderStageCreateInfo& stage : CREATE_INFO.mStages) {
				stage.module = CREATE_INFO.mpShaderModule;
			}

			CREATE_INFO.mVertexInput.vertexBindingDescriptionCount = static_cast<uint32_t>(CREATE_INFO.mVERTEX_BINDINGS.size());
			CREATE_INFO.mVertexInput.pVertexBindingDescriptions = CREATE_INFO.mVERTEX_BINDINGS.data();
			CREATE_INFO.mVertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(CREATE_INFO.mVERTEX_BINDING_ATTRIBUTES.size());
			CREATE_INFO.mVertexInput.pVertexAttributeDescriptions = CREATE_INFO.mVERTEX_BINDING_ATTRIBUTES.data();

			CREATE_INFO.mColorBlend.attachmentCount = static_cast<uint32_t>(CREATE_INFO.mCOLOR_BLEND_ATTACHMENTS.size());
			CREATE_INFO.mColorBlend.pAttachments = CREATE_INFO.mCOLOR_BLEND_ATTACHMENTS.data();

			CREATE_INFO.mDynamicState.dynamicStateCount = static_cast<uint32_t>(CREATE_INFO.mDYNAMIC_STATES.size());
			CREATE_INFO.mDynamicState.pDynamicStates = CREATE_INFO.mDYNAMIC_STATES.data();

			CREATE_INFO.mPipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(CREATE_INFO.mDESCRIPTOR_SET_pLAYOUTS.size());
			CREATE_INFO.mPipelineLayoutInfo.pSetLayouts = CREATE_INFO.mDESCRIPTOR_SET_pLAYOUTS.data();

			CHECK_VK_SUCCESS(
			vkCreatePipelineLayout(devices->getLogicalDevice(), &CREATE_INFO.mPipelineLayoutInfo, nullptr, &CREATE_INFO.mpPipelineLayout),
			"Failed to create pipeline layout"
			)

			CREATE_INFO.mPipelineCreateInfo.pNext = &CREATE_INFO.mRendering;
			CREATE_INFO.mPipelineCreateInfo.stageCount = static_cast<uint32_t>(CREATE_INFO.mStages.size());
			CREATE_INFO.mPipelineCreateInfo.pStages = CREATE_INFO.mStages.data();
			CREATE_INFO.mPipelineCreateInfo.pVertexInputState = &CREATE_INFO.mVertexInput;
			CREATE_INFO.mPipelineCreateInfo.pInputAssemblyState = &CREATE_INFO.mINPUT_ASSEMBLY;
			CREATE_INFO.mPipelineCreateInfo.pTessellationState = &CREATE_INFO.mTESSELLATION;
			CREATE_INFO.mPipelineCreateInfo.pViewportState = &CREATE_INFO.mVIEWPORT;
			CREATE_INFO.mPipelineCreateInfo.pRasterizationState = &CREATE_INFO.mRASTERIZATION;
			CREATE_INFO.mPipelineCreateInfo.pMultisampleState = &CREATE_INFO.mMULTISAMPLE;
			CREATE_INFO.mPipelineCreateInfo.pDepthStencilState = &CREATE_INFO.mDEPTH_STENCIL;
			CREATE_INFO.mPipelineCreateInfo.pColorBlendState = &CREATE_INFO.mColorBlend;
			CREATE_INFO.mPipelineCreateInfo.pDynamicState = &CREATE_INFO.mDynamicState;
			CREATE_INFO.mPipelineCreateInfo.layout = CREATE_INFO.mpPipelineLayout;
		}

		// create the graphics pipeline
		{
			CHECK_VK_SUCCESS(
			vkCreateGraphicsPipelines(devices->getLogicalDevice(), nullptr, 1, &CREATE_INFO.mPipelineCreateInfo, nullptr, &mpGraphicsPipeline),
			"Failed to create graphics pipeline"
			)
		}
	}

	GraphicsPipeline::~GraphicsPipeline() {
		vkDestroyShaderModule(devices->getLogicalDevice(), CREATE_INFO.mpShaderModule, nullptr);
		vkDestroyPipelineLayout(devices->getLogicalDevice(), CREATE_INFO.mpPipelineLayout, nullptr);
		vkDestroyPipeline(devices->getLogicalDevice(), mpGraphicsPipeline, nullptr);
	}

	[[nodiscard]] GraphicsPipeline::GraphicsPipelineConstructInfo GraphicsPipeline::sGetConstructParameters(std::vector<VkDescriptorSetLayout> const& DESCRIPTOR_SET_pLAYOUTS) {
		VkGraphicsPipelineCreateInfo pipelineCreateInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			// REROUTE EVERYTHING NEEDED
		};

		const std::vector<VkFormat> COLOR_ATTACHMENT_FORMATS{ VK_FORMAT_R8G8B8A8_SRGB };
		VkPipelineRenderingCreateInfo rendering{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
			.colorAttachmentCount = 0, // reroute needed
			.pColorAttachmentFormats = nullptr, // reroute needed
			.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT,
			.stencilAttachmentFormat = {},
		};

		const std::vector<char> SHADERS_SPRIV_FILE_BYTES{ Common::fLoadSprivFileBytes(R"(C:\Users\paulp\ComputerPrograms\Renderer6\shaders\shaders.spv)") };
		VkShaderModule shaderModuleForEverything{}; // create needed
		VkShaderModuleCreateInfo shaderModuleForEverythingInfo{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = 0, // reroute needed
			.pCode = nullptr // reroute needed
		};
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos{
			VkPipelineShaderStageCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_VERTEX_BIT,
				.module = nullptr, // reroute needed
				.pName = "vertexShader"
			},
			VkPipelineShaderStageCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
				.module = nullptr, // reroute needed
				.pName = "fragmentShader"
			}
		};

		const std::vector<VkVertexInputBindingDescription> VERTEX_BINDINGS{ Vertex::Vertex::sGetInputBindingDescription() };
		const std::vector<VkVertexInputAttributeDescription> VERTEX_BINDING_ATTRIBUTES{ Vertex::Vertex::sGetInputAttributeDescriptions() };
		VkPipelineVertexInputStateCreateInfo vertexInput{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = 0, // reroute needed
			.pVertexBindingDescriptions = nullptr, // reroute needed
			.vertexAttributeDescriptionCount = 0, // reroute needed
			.pVertexAttributeDescriptions = nullptr, // reroute needed
		};

		const VkPipelineInputAssemblyStateCreateInfo INPUT_ASSEMBLY{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		// UNUSED CURRENTLY
		const VkPipelineTessellationStateCreateInfo TESSELLATION{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO
		};

		const VkPipelineViewportStateCreateInfo VIEWPORT{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = nullptr, // will dynamically set
			.scissorCount = 1,
			.pScissors = nullptr // will dynamically set
		};

		const VkPipelineRasterizationStateCreateInfo RASTERIZATION{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 1.0f,
			.lineWidth = 1.0f
		};

		// UNUSED CURRENTLY
		const VkPipelineMultisampleStateCreateInfo MULTISAMPLE{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE,
		};

		const VkPipelineDepthStencilStateCreateInfo DEPTH_STENCIL{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE,
		};

		// UNUSED CURRENTLY
		const std::vector<VkPipelineColorBlendAttachmentState> COLOR_BLEND_ATTACHMENTS{
			VkPipelineColorBlendAttachmentState{
				.blendEnable = VK_FALSE,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT 
			}
		};
		VkPipelineColorBlendStateCreateInfo colorBlend{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,	
			.attachmentCount = 0, // reroute needed
			.pAttachments = nullptr, // reroute needed
		};

		const std::vector<VkDynamicState> DYNAMIC_STATES{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 0, // reroute needed
			.pDynamicStates = nullptr, // reroute needed
		};

		VkPipelineLayoutCreateInfo layoutInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 0, // reroute needed
			.pSetLayouts = nullptr // reroute needed
		};
		VkPipelineLayout returnpPipelineLayout{}; // creation needed

		return GraphicsPipelineConstructInfo{
			pipelineCreateInfo,
			COLOR_ATTACHMENT_FORMATS,
			rendering,
			SHADERS_SPRIV_FILE_BYTES,
			shaderModuleForEverythingInfo,
			shaderModuleForEverything,
			shaderStageInfos,
			VERTEX_BINDINGS,
			VERTEX_BINDING_ATTRIBUTES,
			vertexInput,
			INPUT_ASSEMBLY,
			TESSELLATION,
			VIEWPORT,
			RASTERIZATION,
			MULTISAMPLE,
			DEPTH_STENCIL,
			COLOR_BLEND_ATTACHMENTS,
			colorBlend,
			DYNAMIC_STATES,
			dynamicState,
			DESCRIPTOR_SET_pLAYOUTS,
			layoutInfo,
			returnpPipelineLayout
		};
	}
}