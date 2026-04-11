#include "GraphicsPipeline.h"
#include "Util.h"
#include "LogicalDevice.h"

namespace Engine {
	namespace GraphicsPipeline {
		void populate() {
			populateGraphicsCreates();
			populateComputeCreates();
			createPipelines();
		}

		void populateGraphicsCreates() {

		}

		void populateComputeCreates() {

		}

		void createPipelines() {

		}

		VkPipeline newGraphicsPipeline(VkGraphicsPipelineCreateInfo const& CREATE) {
			VkPipeline graphics{};

			CHECK_VK_SUCCESS(vkCreateGraphicsPipelines(gpDevice, VK_NULL_HANDLE, 1, &CREATE, nullptr, &graphics), "Failed to create graphics pipeline")
			pipelines.push_back(graphics);

			return graphics;
		}

		VkPipeline newComputePipeline(VkComputePipelineCreateInfo const& CREATE) {
			VkPipeline compute{};

			CHECK_VK_SUCCESS(vkCreateComputePipelines(gpDevice, VK_NULL_HANDLE, 1, &CREATE, nullptr, &compute), "Failed to create compute pipeline")
			pipelines.push_back(compute);

			return compute;
		}

		void clear() noexcept {
			for(VkPipeline& pipeline : pipelines) {
				vkDestroyPipeline(gpDevice, pipeline, nullptr);
			}
		}
	}
}