#include "PipelineLayout.h"
#include "LogicalDevice.h"

namespace Engine {
	namespace PipelineLayout {
		VkPipelineLayout newLayout(VkPipelineLayoutCreateInfo const& CREATE) {
			VkPipelineLayout layout{};

			CHECK_VK_SUCCESS(vkCreatePipelineLayout(gpDevice, &CREATE, nullptr, &layout), "Failed to create pipeline layout")
			layouts.push_back(layout);

			return layout;
		}

		void clear() noexcept {
			for(VkPipelineLayout& layout : layouts) {
				vkDestroyPipelineLayout(gpDevice, layout, nullptr);
			}
		}
	}
}