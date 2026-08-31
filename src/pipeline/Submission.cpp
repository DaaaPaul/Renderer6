#include <algorithm>
#include "Submission.hpp"
#include "backend/Swapchain.h"
#include "Memory/MemoryManager.h"
#include "utility/UtilityStructs.hpp"

Attachments::Attachments(uint32_t sc_image_index, std::optional<VkClearColorValue> clear_color, std::optional<VkClearDepthStencilValue> clear_depth) :
	rendering_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = VkRect2D{ VkOffset2D{0, 0}, Swapchain::g_status.imageExtent },
			.layerCount = 1,
	}, 
	sc_image{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = Swapchain::g_image_views[sc_image_index],
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.loadOp = (clear_color.has_value()) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = (clear_color.has_value()) ? VkClearValue{ .color = clear_color.value() } : VkClearValue{}	
	},
	depth_image{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = MemoryManager::g_image_views.get("depth view")->get_image_view(),
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.loadOp = (clear_depth.has_value()) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = (clear_depth.has_value()) ? VkClearValue{ .depthStencil = clear_depth.value() } : VkClearValue{}
	} {
	point(&rendering_info, &sc_image, &depth_image);
}

void Attachments::point(VkRenderingInfo* p_rendering_info, const VkRenderingAttachmentInfo* p_SC_IMAGE, const VkRenderingAttachmentInfo* p_DEPTH_IMAGE) {
	p_rendering_info->colorAttachmentCount = 1;
	p_rendering_info->pColorAttachments = p_SC_IMAGE;
	p_rendering_info->pDepthAttachment = p_DEPTH_IMAGE;
}

void Submission::record(Submission* submission) {
	Vulkan::begin_cmd_buffer(submission->cmd_buf, Vulkan::NO_FLAGS);

	if(submission->f_set_cmds) {
		submission->f_set_cmds(submission->cmd_buf);
	}
	if(submission->f_bind_cmds) {
		submission->f_bind_cmds(submission->cmd_buf);
	}

	std::for_each(submission->pre_render_barriers.begin(), submission->pre_render_barriers.end(), ImageBarrier::insert);

	Attachments attachments(submission->sc_image_index, submission->clear_color, submission->clear_depth);
	Attachments::begin_rendering(submission->cmd_buf, &attachments);
	
	if(submission->f_render_cmds) {
		submission->f_render_cmds(submission->cmd_buf);
	}

	Attachments::end_rendering(submission->cmd_buf);

	std::for_each(submission->post_render_barriers.begin(), submission->post_render_barriers.end(), ImageBarrier::insert);

	Vulkan::end_cmd_buffer(submission->cmd_buf);
}

void Submission::submit(VkQueue queue, VkFence fence, const std::vector<Submission>* p_SUBMISSIONS) {
	std::vector<CmdBufSubmit> cmd_buf_submits;
	cmd_buf_submits.reserve(p_SUBMISSIONS->size());

	for(int i = 0; i < p_SUBMISSIONS->size(); ++i) {
		cmd_buf_submits.emplace_back((*p_SUBMISSIONS)[i].cmd_buf);
	}

	std::vector<VkSubmitInfo2> submit_info2s(CmdBufSubmit::get_submit_info2s(&cmd_buf_submits));

	vkQueueSubmit2(queue, submit_info2s.size(), submit_info2s.data(), fence);
}