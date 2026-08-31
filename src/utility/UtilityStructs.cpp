#include "UtilityStructs.hpp"

CmdBufSubmit::CmdBufSubmit(VkCommandBuffer cmd_buf) :
	submit_info2{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.commandBufferInfoCount = 1,
		.pCommandBufferInfos = &cmd_buf_info
	},
	cmd_buf_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = cmd_buf
	} {
}

std::vector<VkSubmitInfo2> CmdBufSubmit::get_submit_info2s(const std::vector<CmdBufSubmit>* p_CMD_BUF_SUBMITS) {
	std::vector<VkSubmitInfo2> submit_info2s;
	submit_info2s.reserve(p_CMD_BUF_SUBMITS->size());

	for(int i = 0; i < p_CMD_BUF_SUBMITS->size(); ++i) {
		submit_info2s.push_back((*p_CMD_BUF_SUBMITS)[i].submit_info2);
	}

	return submit_info2s;
}