#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include "FramesInFlight.hpp"
#include "Backend/Swapchain.h"
#include "ShaderStructs/UniformBufferBlock.hpp"

namespace Engine {	
	struct ScAcquire {
		uint32_t sc_image_index{};
		VkResult result{};
	};

	inline float g_total_delta_time = 0.0f;
	inline uint32_t g_total_loops = 0;
	inline UniformBufferBlock g_ubo_data;
	inline glm::vec2 g_circle_position{};

	void record_draw_model(VkCommandBuffer cmd_buf, uint32_t sc_image_index);
	void record_draw_simple(VkCommandBuffer cmd_buf, uint32_t sc_image_index);
	void render_next(Frame& frame);
	void run();

	ScAcquire acquire_sc_image(VkFence fence_to_signal);
	VkResult present_sc_image(uint32_t sc_image_index, VkQueue queue);
	void wait_timeline_semaphore(VkSemaphore semaphore, uint64_t wait_val);
	void wait_fence(VkFence fence);
	void set_viewport_and_scissor(VkCommandBuffer cmd_buf);
	void resize();
	void check_close();
	void update(float delta_time);
}
