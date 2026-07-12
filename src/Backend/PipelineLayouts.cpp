#include "Backend/PipelineLayouts.h"
#include "Backend/LogicalDevice.h"
#include "Memory/MemoryManager.h"
#include "Utility/Vulkan.h"
#include "ShaderStructs/PushConstantBlock.hpp"

namespace PipelineLayouts {
	void init() {
		{
			std::vector<VkDescriptorSetLayout> descriptor_set_layouts{
				MemoryManager::g_descriptor_set.get_layout() // p_ktx_texture image and sampler
			};
			std::vector<VkPushConstantRange> push_constant_ranges{
				VkPushConstantRange{
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
					.offset = 0,
					.size = 8
				}
			};
			g_layouts.push_back(Vulkan::create_pipeline_layout(descriptor_set_layouts, push_constant_ranges));
		}

		{
			std::vector<VkDescriptorSetLayout> descriptor_set_layouts{
				// yayayaya add me
			};
			std::vector<VkPushConstantRange> push_constant_ranges{
				VkPushConstantRange{
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
					.offset = 8,
					.size = sizeof(PushConstantBlock)
				}
			};
			g_layouts.push_back(Vulkan::create_pipeline_layout(descriptor_set_layouts, push_constant_ranges));
		}
	}

	void clear() {
		for(VkPipelineLayout layout : g_layouts) {
			vkDestroyPipelineLayout(g_device, layout, nullptr);
		}
		g_layouts.clear();
	}
}
