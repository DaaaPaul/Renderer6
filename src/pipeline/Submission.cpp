#include <algorithm>
#include "Submission.hpp"
#include "backend/Swapchain.h"
#include "Memory/MemoryManager.h"

Attachments::Attachments(uint32_t sc_image_index, const VkClearColorValue* p_CLEAR_COLOR, const VkClearDepthStencilValue* p_CLEAR_DEPTH) :
	rendering_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = VkRect2D{ VkOffset2D{0, 0}, Swapchain::g_status.imageExtent },
			.layerCount = 1,
	}, 
	sc_image{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = Swapchain::g_image_views[sc_image_index],
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.loadOp = (p_CLEAR_COLOR) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = (p_CLEAR_COLOR) ? VkClearValue{ .color = *p_CLEAR_COLOR } : VkClearValue{}	
	},
	depth_image{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = MemoryManager::g_image_views.get("depth view")->get_image_view(),
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.loadOp = (p_CLEAR_DEPTH) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = (p_CLEAR_DEPTH) ? VkClearValue{ .depthStencil = *p_CLEAR_DEPTH } : VkClearValue{}
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

	submission->f_set_cmds(submission->cmd_buf);
	submission->f_bind_cmds(submission->cmd_buf);

	std::for_each(submission->pre_render_barriers.begin(), submission->pre_render_barriers.end(), ImageBarrier::insert);

	Attachments attachments(submission->sc_image_index, submission->p_CLEAR_COLOR, submission->p_CLEAR_DEPTH);
	Attachments::begin_rendering(submission->cmd_buf, &attachments);
	
	submission->f_render_cmds(submission->cmd_buf);

	Attachments::end_rendering(submission->cmd_buf);

	std::for_each(submission->post_render_barriers.begin(), submission->post_render_barriers.end(), ImageBarrier::insert);

	Vulkan::end_cmd_buffer(submission->cmd_buf);
}