#pragma once

#include <vulkan/vulkan.h>

struct CmdBufSubmit {
	VkSubmitInfo2 submit_info2{};
	VkCommandBufferSubmitInfo cmd_buf_info{};

	explicit CmdBufSubmit(VkCommandBuffer cmd_buf);
};

struct Index {
	uint32_t wrap{};
	uint32_t val{};

	explicit Index(uint32_t wrap, uint32_t initial_val) :
		wrap{ wrap },
		val{ initial_val } {}
	void increase() {
		val = (val + 1) % wrap;
	}
};