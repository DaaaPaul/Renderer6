#pragma once

#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <chrono>
#include "Camera.hpp"

namespace Engine {
	void insert_image_barrier(VkCommandBuffer cmd_buf, VkImage image, const VkImageSubresourceRange& subresource_range, const VkPipelineStageFlags2& stage1, const VkAccessFlags2& access1, const VkPipelineStageFlags2& stage2, const VkAccessFlags2& access2, const VkImageLayout& old_layout, const VkImageLayout& new_layout, const uint32_t& graphics_queue_family_index);
	
	void record_compute(VkCommandBuffer cmd_buf);
	void record_draw_model(VkCommandBuffer cmd_buf, const uint32_t& sc_image_index);
	void record_draw_particles(VkCommandBuffer cmd_buf, const uint32_t& sc_image_index);
	void render_next();
	void run();

	bool acquire_sc_image(uint32_t& sc_image_index, VkFence fence_to_signal);
	bool present_sc_image(const uint32_t& sc_image_index, VkQueue queue);
	void wait_timeline_semaphore(VkSemaphore semaphore, const uint64_t& wait_val);
	void wait_fence(VkFence fence);
	void begin_cmd_buf(VkCommandBuffer cmd_buf);
	void set_viewport_and_scissor(VkCommandBuffer cmd_buf);
	void resize();
	void check_close();
	void update(const float&);
	float get_delta_time(const std::chrono::steady_clock::time_point&, const std::chrono::steady_clock::time_point&);
}
