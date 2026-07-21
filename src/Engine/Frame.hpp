#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <cassert>
#include "Backend/Swapchain.h"
#include "Backend/LogicalDevice.h"
#include "Utility/Utility.h"

struct Submit {
	VkCommandPool cmd_pool{};
	VkSemaphore timeline{};

	VkSemaphoreSubmitInfo wait{};
	VkCommandBufferSubmitInfo cmd{};
	VkSemaphoreSubmitInfo signal{};

	VkSubmitInfo2 submit{};

	explicit Submit(VkCommandPool cmd_pool, VkSemaphore timeline);

	~Submit() noexcept;

	DELETE_COPYING_MOVING(Submit)
};

struct SubmitArgs {
	VkCommandPool cmd_pool{};
	VkSemaphore timeline{};

	operator Submit() {
		return Submit(cmd_pool, timeline);
	}
};

struct Frame {
	VkCommandPool cmd_pool{};

	VkFence fence{};
	VkSemaphore timeline{};
	uint64_t timeline_val{};
	std::vector<SubmitArgs> args;
	std::vector<Submit> submits;

	explicit Frame(VkCommandPool cmd_pool, uint32_t submit_count);

	void progress_timeline() {
		for(Submit& submit : submits) {
			submit.wait.value = timeline_val;
			submit.signal.value = ++timeline_val;
		}
	}

	~Frame() noexcept;

	DELETE_COPYING_MOVING(Frame)
};

struct FrameArgs {
	VkCommandPool cmd_pool{};
	uint32_t submit_count{};

	operator Frame() {
		return Frame(cmd_pool, submit_count);
	}
};