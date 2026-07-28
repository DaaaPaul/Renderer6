#include "pipeline/PipelineLayouts.h"
#include "backend/LogicalDevice.h"
#include "memory/MemoryManager.h"
#include "utility/Vulkan.h"
#include "shader/PushConstantBlock.hpp"
#include "Gui/Gui.h"

namespace PipelineLayouts {
	void init() {
		{
			std::vector<VkDescriptorSetLayout> descriptor_set_layouts{
				MemoryManager::g_descriptor_sets.get("descriptor set")->get_layout()
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

		{
			std::vector<VkDescriptorSetLayout> descriptor_set_layouts{
				MemoryManager::g_descriptor_sets.get("gui descriptor set")->get_layout()
			};
			std::vector<VkPushConstantRange> push_constant_ranges{
				VkPushConstantRange{
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
					.offset = 0,
					.size = sizeof(Gui::PushConstantBlock)
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
