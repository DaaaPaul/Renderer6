#include "GraphicsPipeline.h"
#include "Util.h"
#include "LogicalDevice.h"

namespace Engine {
	namespace GraphicsPipeline {
		VkPipeline newLayout(VkGraphicsPipelineCreateInfo const& CREATE) {
			VkPipeline graphics{};

			CHECK_VK_SUCCESS(vkCreateGraphicsPipelines(gpDevice, VK_NULL_HANDLE, 1, &CREATE, nullptr, &graphics), "Failed to create graphics pipeline")
			pipelines.push_back(graphics);

			return graphics;
		}

		void clear() noexcept {
			for(VkPipeline& graphics : pipelines) {
				vkDestroyPipeline(gpDevice, graphics, nullptr);
			}
		}
	}
}