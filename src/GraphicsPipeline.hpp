#pragma once

#include "Devices.hpp"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "Common.h"

namespace Engine {
	struct GraphicsPipeline {
		struct GraphicsPipelineConstructInfo {
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
			const std::vector<VkDescriptorSetLayout> mDESCRIPTOR_SET_pLAYOUTS{};
			VkPipelineLayoutCreateInfo mPipelineLayoutInfo{};
			VkPipelineLayout mpPipelineLayout{};
		};

		Backend::Devices* devices{};
		VkPipeline mpGraphicsPipeline{};
		GraphicsPipelineConstructInfo CREATE_INFO{};

		[[nodiscard]] static GraphicsPipelineConstructInfo sGetConstructParameters(std::vector<VkDescriptorSetLayout> const& DESCRIPTOR_SET_LAYOUTS);

		GraphicsPipeline();
		explicit GraphicsPipeline(Backend::Devices* givenDevices, GraphicsPipelineConstructInfo const& GIVEN_VULKAN_GRAPHICS_PIPELINE_WRAPPER_CONSTRUCT_INFO);
		~GraphicsPipeline();

		DELETE_COPY_CONSTRUCTORS(GraphicsPipeline)
		DELETE_MOVE_CONSTRUCTORS(GraphicsPipeline)
	};
}