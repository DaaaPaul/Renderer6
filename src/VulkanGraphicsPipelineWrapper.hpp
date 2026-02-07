#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "VulkanHostVisibleMemory.hpp"
#include "VulkanDeviceLocalMemory.hpp"
#include "Common.h"

struct VulkanGraphicsPipelineWrapper {
    struct VulkanGraphicsPipelineWrapperConstructInfo {
		VkGraphicsPipelineCreateInfo mPipelineCreateInfo{};
		const std::vector<VkFormat> mCOLOR_ATTACHMENT_FORMATS{};
		VkPipelineRenderingCreateInfo mRendering{};
		const std::vector<char> mSHADERS_SPRIV_FILE_BYTES{};
		VkShaderModuleCreateInfo mShaderModuleCreateInfo{};
		VkShaderModule mpShaderModule{};
		std::vector<VkPipelineShaderStageCreateInfo> mStages{};
		const std::vector<VkVertexInputBindingDescription> mVERTEX_BINDINGS{};
		const std::vector<VkVertexInputAttributeDescription> mVERTEX_BINDING_ATTRIBUTES{};
		VkPipelineVertexInputStateCreateInfo mVertexInput{};
		const VkPipelineInputAssemblyStateCreateInfo mINPUT_ASSEMBLY{};
		const VkPipelineTessellationStateCreateInfo mTESSELLATION{};
		const VkPipelineViewportStateCreateInfo mVIEWPORT{};
		const VkPipelineRasterizationStateCreateInfo mRASTERIZATION{};
		const VkPipelineMultisampleStateCreateInfo mMULTISAMPLE{};
		const VkPipelineDepthStencilStateCreateInfo mDEPTH_STENCIL{};
		const std::vector<VkPipelineColorBlendAttachmentState> mCOLOR_BLEND_ATTACHMENTS{};
		VkPipelineColorBlendStateCreateInfo mColorBlend{};
		const std::vector<VkDynamicState> mDYNAMIC_STATES{};
		VkPipelineDynamicStateCreateInfo mDynamicState{};
		const VkPipelineLayoutCreateInfo mPIPELINE_LAYOUT_INFO{};
		VkPipelineLayout mpPipelineLayout{};
    };

    VulkanDevicesWrapper* mpVulkanDevicesWrapper{};
	VkPipeline mpGraphicsPipeline{};
	VulkanGraphicsPipelineWrapperConstructInfo mParameters{};

    [[nodiscard]] static VulkanGraphicsPipelineWrapperConstructInfo getConstructParameters(VkPipelineLayoutCreateInfo const& PIPLINE_LAYOUT_INFO);

    explicit VulkanGraphicsPipelineWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper, VulkanGraphicsPipelineWrapperConstructInfo const& GIVEN_VULKAN_GRAPHICS_PIPELINE_WRAPPER_CONSTRUCT_INFO);
    ~VulkanGraphicsPipelineWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanGraphicsPipelineWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanGraphicsPipelineWrapper)
};