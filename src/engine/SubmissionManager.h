#pragma once

#include "pipeline/Submission.hpp"
#include "containers/Addmap.hpp"
#include "backend/Swapchain.h"
#include "utility/Vulkan.h"

namespace SubmissionManager {
	inline VkCommandPool g_cmd_pool{};
	inline Addmap<Submission> g_submissions(Swapchain::g_IMAGE_COUNT * 2);

	void init();
	void destroy();

	inline void set_cmd_pool() {
		g_cmd_pool = Vulkan::create_cmd_pool(Vulkan::NO_FLAGS, PhysicalDevice::g_graphics_family_index[0]);
	}
	inline void destroy_cmd_pool() {
		vkDestroyCommandPool(g_device, g_cmd_pool, nullptr);
	}

	void set_submissions();
	void record_submissions();
	void destroy_submissions();

	void set_sc_viewport(VkCommandBuffer cmd_buf);
	void set_sc_scissor(VkCommandBuffer cmd_buf);
}