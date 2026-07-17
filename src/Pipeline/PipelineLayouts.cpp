#include "Pipeline/PipelineLayouts.h"
#include "Backend/LogicalDevice.h"
#include "Memory/MemoryManager.h"
#include "Utility/Vulkan.h"
#include "Utility/Ids.h"
#include "ShaderStructs/PushConstantBlock.hpp"

namespace PipelineLayouts {
	void init() {
		{
			std::vector<VkDescriptorSetLayout> descriptor_set_layouts{
				MemoryManager::g_descriptor_sets.get<DescriptorSet>(Ids::g_DESCRIPTOR_SET)->get_layout()
			};
			std::vector<VkPushConstantRange> push_constant_ranges{
				VkPushConstantRange{
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
					.offset = 0,
					.size = sizeof(PushConstantBlock)
				}
			};

			g_layouts.push_back(Vulkan::create_pipeline_layout(descriptor_set_layouts, push_constant_ranges));
		}

		{
			std::vector<VkPushConstantRange> push_constant_ranges{
				VkPushConstantRange{
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
					.offset = 0,
					.size = sizeof(glm::mat4) * 3
				}
			};

			g_layouts.push_back(Vulkan::create_pipeline_layout({}, push_constant_ranges));
		}
	}

	void clear() {
		for(VkPipelineLayout layout : g_layouts) {
			vkDestroyPipelineLayout(g_device, layout, nullptr);
		}
		g_layouts.clear();
	}
}
