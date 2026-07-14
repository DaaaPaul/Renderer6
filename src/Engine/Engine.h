#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include "Engine/CameraComponent.hpp"

namespace Engine {	
	struct Acquire {
		uint32_t sc_image_index{};
		VkResult result{};
	};

	inline float total_delta_time = 0.0f;
	inline uint32_t total_loops = 0;

	void record_draw_model(VkCommandBuffer cmd_buf, uint32_t sc_image_index);
	void render_next();
	void run();

	Acquire acquire_sc_image(VkFence fence_to_signal);
	VkResult present_sc_image(uint32_t sc_image_index, VkQueue queue);
	void wait_timeline_semaphore(VkSemaphore semaphore, uint64_t wait_val);
	void wait_fence(VkFence fence);
	void set_viewport_and_scissor(VkCommandBuffer cmd_buf);
	void resize();
	void check_close();
	void update(float delta_time);
}
