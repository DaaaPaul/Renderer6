#include "FrameKits.h"
#include "Util.h"
#include "PhysicalDevice.h"

namespace FrameKits {
	void init() {
		g_cmd_pool = create_cmd_pool();
		g_frame_kits = create_frame_kits();
	}

	void destroy() {
		g_frame_kits.clear();
		vkDestroyCommandPool(g_device, g_cmd_pool, nullptr);
	}

	void recreate() {
		g_frame_index = 0;
		destroy();
		init();
	}

	VkCommandPool create_cmd_pool() {
		return Vulkan::create_cmd_pool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, PhysicalDevice::g_queue_family_indices[0]);
	}

	std::vector<FrameKit> create_frame_kits() {
		std::vector<FrameKit> frame_kits{};
		frame_kits.reserve(g_FRAMES_IN_FLIGHT);

		for(int i = 0; i < g_FRAMES_IN_FLIGHT; ++i) {
			VkSemaphore shared_semaphore = Vulkan::create_semaphore(VK_SEMAPHORE_TYPE_TIMELINE);

			SubmitKit compute_shader_submit(shared_semaphore, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, Vulkan::create_cmd_buffer(g_cmd_pool));
			SubmitKit model_draw_submit(shared_semaphore, VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, Vulkan::create_cmd_buffer(g_cmd_pool));
			SubmitKit particles_draw_submit(shared_semaphore, VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, Vulkan::create_cmd_buffer(g_cmd_pool));
			std::vector<SubmitKit> submit_kits{ compute_shader_submit, model_draw_submit, particles_draw_submit };

			g_frame_kits.emplace_back(
				SyncKit(Vulkan::create_fence(VK_NO_FLAGS), shared_semaphore), 
				submit_kits,
				std::vector<SyncPair>(submit_kits.size())
			);
		}

		return frame_kits;
	}
}
