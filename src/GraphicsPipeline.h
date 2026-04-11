#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Engine {
	namespace GraphicsPipeline {
		std::vector<VkPipeline> pipelines{};
		std::vector<VkGraphicsPipelineCreateInfo> graphicsCreates{};
		std::vector<VkComputePipelineCreateInfo> computeCreates{};

		void populate();

		void populateGraphicsCreates() noexcept;
		void populateComputeCreates() noexcept;
		void createPipelines();

		VkPipeline newGraphicsPipeline(VkGraphicsPipelineCreateInfo const&);
		VkPipeline newComputePipeline(VkComputePipelineCreateInfo const&);
		void clear() noexcept;
	}
}