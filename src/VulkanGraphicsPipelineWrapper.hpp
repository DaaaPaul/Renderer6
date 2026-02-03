#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "Common.h"

struct VulkanGraphicsPipelineWrapper {
    struct VulkanGraphicsPipelineWrapperConstructInfo {
		VkGraphicsPipelineCreateInfo mPipelineCreateInfo{};
		const std::vector<VkFormat> mCOLOR_ATTACHMENT_FORMATS{};
		const VkPipelineRenderingCreateInfo mRENDERING{};
		const VkShaderModule mShaderModule{};
		const std::vector<VkPipelineShaderStageCreateInfo> mSTAGES{};
		const VkPipelineVertexInputStateCreateInfo mVERTEX_INPUT{};
		const VkPipelineInputAssemblyStateCreateInfo mINPUT_ASSEMBLY{};
		const VkPipelineTessellationStateCreateInfo mTESSELLATION{};
		const VkPipelineViewportStateCreateInfo mVIEWPORT{};
		const VkPipelineRasterizationStateCreateInfo mRASTERIZATION{};
		const VkPipelineMultisampleStateCreateInfo mMULTISAMPLE{};
		const VkPipelineDepthStencilStateCreateInfo mDEPTH_STENCIL{};
		const VkPipelineColorBlendStateCreateInfo mCOLOR_BLEND{};
		const VkPipelineDynamicStateCreateInfo mDYNAMIC_STATE{};
		const VkPipelineLayout mPipelineLayout{};
    };

    VulkanDevicesWrapper* mpVulkanDevicesWrapper{};
	VkPipeline mpGraphicsPipeline{};
	VulkanGraphicsPipelineWrapperConstructInfo mParameters{};

    [[nodiscard]] static VulkanGraphicsPipelineWrapperConstructInfo getConstructParameters(std::string const& VERTEX_AND_FRAGMENT_SPRIV_PATH);

    explicit VulkanGraphicsPipelineWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper, VulkanGraphicsPipelineWrapperConstructInfo const& GIVEN_VULKAN_GRAPHICS_PIPELINE_WRAPPER_CONSTRUCT_INFO);
    ~VulkanGraphicsPipelineWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanGraphicsPipelineWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanGraphicsPipelineWrapper)
};