#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct CmdBufSubmit {
	VkSubmitInfo2 submit_info2{};
	VkCommandBufferSubmitInfo cmd_buf_info{};

	explicit CmdBufSubmit(VkCommandBuffer cmd_buf);

	static std::vector<VkSubmitInfo2> get_submit_info2s(const std::vector<CmdBufSubmit>* p_CMD_BUF_SUBMITS);
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