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