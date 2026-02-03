#include "VulkanGraphicsPipelineWrapper.hpp"

explicit VulkanGraphicsPipelineWrapper::VulkanGraphicsPipelineWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper, VulkanGraphicsPipelineWrapperConstructInfo const& GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO) :
	mpVulkanDevicesWrapper{ givenVulkanDevicesWrapper },
	mpGraphicsPipeline{},
	mParameters{ GIVEN_VULKAN_SWAPCHAIN_WRAPPER_CONSTRUCT_INFO } {

}

VulkanGraphicsPipelineWrapper::~VulkanGraphicsPipelineWrapper() {

}

[[nodiscard]] VulkanGraphicsPipelineWrapper::VulkanGraphicsPipelineWrapperConstructInfo VulkanGraphicsPipelineWrapper::getConstructParameters(std::string const& VERTEX_AND_FRAGMENT_SPRIV_PATH) {
	VkGraphicsPipelineCreateInfo PipelineCreateInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.flags = 0,
	};

	const std::vector<VkFormat> FORMAT{ VK_FORMAT_R8G8B8A8_SRGB };
	const VkPipelineRenderingCreateInfo RENDERING{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = 0, // reroute needed
		.pColorAttachmentFormats = nullptr, // reroute needed
		.depthAttachmentFormat = {},
		.stencilAttachmentFormat = {},
	};

	const std::vector<char> SHADERS_SPRIV_FILE_BYTES{ Common::fLoadSprivFileBytes(VERTEX_AND_FRAGMENT_SPRIV_PATH) };
	const VkShaderModule SHADER_MODULE_FOR_EVERYTHING{};
	const VkShaderModuleCreateInfo SHADER_MODULE_FOR_EVERYTHING_INFO{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = static_cast<uint32_t>(SHADERS_SPRIV_FILE_BYTES.size()),
		.pCode = reinterpret_cast<uint32_t const*>(SHADERS_SPRIV_FILE_BYTES.data())
	};
	const std::vector<VkPipelineShaderStageCreateInfo> SHADER_STAGE_INFOS{
		VkPipelineShaderStageCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = nullptr, // reroute needed
			.pName = "vertexShader"
		},
		VkPipelineShaderStageCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = nullptr, // reroute needed
			.pName = "fragmentShader"
		}
	};

	const VkPipelineVertexInputStateCreateInfo VERTEX_INPUT{
		.sType = ,
		.pNext = ,
		.flags = ,
		.vertexBindingDescriptionCount = ,
		.pVertexBindingDescriptions = ,
		.vertexAttributeDescriptionCount = ,
		.pVertexAttributeDescriptions = ,
	};

	const VkPipelineInputAssemblyStateCreateInfo INPUT_ASSEMBLY{
		.sType = ,
		.pNext = ,
		.flags = ,
		.topology = ,
		.primitiveRestartEnable = ,
	};

	const VkPipelineTessellationStateCreateInfo TESSELLATION{};

	const VkPipelineViewportStateCreateInfo VIEWPORT{
		.sType = ,
		.pNext = ,
		.flags = ,
		.viewportCount = ,
    	.pViewports = ,
		.scissorCount = ,
		.pScissors = ,
	};

	const VkPipelineRasterizationStateCreateInfo RASTERIZATION{
		.sType = ,
		.pNext = ,
		.flags = ,
		.depthClampEnable = ,
		.rasterizerDiscardEnable = ,
		.polygonMode = ,
		.cullMode = ,
		.frontFace = ,
		.depthBiasEnable = ,
		.depthBiasConstantFactor = ,
		.depthBiasClamp = ,
		.depthBiasSlopeFactor = ,
		.lineWidth = ,
	};

	const VkPipelineMultisampleStateCreateInfo MULTISAMPLE{
	
	};

	const VkPipelineDepthStencilStateCreateInfo DEPTH_STENCIL{
	
	};

	const VkPipelineColorBlendStateCreateInfo COLOR_BLEND{
	
	};

	const VkPipelineDynamicStateCreateInfo DYNAMIC_STATE{
		.sType = ,
		.pNext = ,
		.flags = ,
		.dynamicStateCount = ,
		.pDynamicStates = ,
	};

	const VkPipelineLayout pipelineLayout{
	
	};
}