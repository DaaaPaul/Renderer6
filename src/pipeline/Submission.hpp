#pragma once

#include <vulkan/vulkan.h>
#include <functional>
#include "backend/PhysicalDevice.h"

struct ImageBarrier {
	VkCommandBuffer cmd_buf{};
	VkImage image{};
	VkImageSubresourceRange image_subresource_range{};

	VkPipelineStageFlags2 sync_stage_1{};
	VkAccessFlags2 access_stage_1{};
	VkPipelineStageFlags2 sync_stage_2{};
	VkAccessFlags2 access_stage_2{};

	VkImageLayout old_layout{};
	VkImageLayout new_layout{};

	static void insert(const ImageBarrier& IMAGE_BARRIER) {
		VkImageMemoryBarrier2 insert_image_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = IMAGE_BARRIER.sync_stage_1,
			.srcAccessMask = IMAGE_BARRIER.access_stage_1,
			.dstStageMask = IMAGE_BARRIER.sync_stage_2,
			.dstAccessMask = IMAGE_BARRIER.access_stage_2,
			.oldLayout = IMAGE_BARRIER.old_layout,
			.newLayout = IMAGE_BARRIER.new_layout,
			.srcQueueFamilyIndex = PhysicalDevice::g_graphics_family_index[0],
			.dstQueueFamilyIndex = PhysicalDevice::g_graphics_family_index[0],
			.image = IMAGE_BARRIER.image,
			.subresourceRange = IMAGE_BARRIER.image_subresource_range,
		};

		VkDependencyInfo dependencies{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &insert_image_barrier,
		};

		vkCmdPipelineBarrier2(IMAGE_BARRIER.cmd_buf, &dependencies);
	}
};

struct Attachments {
	VkRenderingInfo rendering_info{};
	VkRenderingAttachmentInfo sc_image{};
	VkRenderingAttachmentInfo depth_image{};

	Attachments(uint32_t sc_image_index, const VkClearColorValue* p_CLEAR_COLOR, const VkClearDepthStencilValue* p_CLEAR_DEPTH);
	static void point(VkRenderingInfo* p_rendering_info, const VkRenderingAttachmentInfo* p_SC_IMAGE, const VkRenderingAttachmentInfo* p_DEPTH_IMAGE);

	static void begin_rendering(VkCommandBuffer cmd_buf, const Attachments* p_ATTACHMENTS) {
		vkCmdBeginRendering(cmd_buf, &p_ATTACHMENTS->rendering_info);
	}
	static void end_rendering(VkCommandBuffer cmd_buf) {
		vkCmdEndRendering(cmd_buf);
	}
};

struct Submission {
	VkCommandBuffer cmd_buf{};

	uint32_t sc_image_index{};
	VkClearColorValue clear_color{};
	const VkClearColorValue* p_CLEAR_COLOR{};
	VkClearDepthStencilValue clear_depth{};
	const VkClearDepthStencilValue* p_CLEAR_DEPTH{};

	std::function<void(VkCommandBuffer)> f_set_cmds{};
	std::function<void(VkCommandBuffer)> f_bind_cmds{};

	std::vector<ImageBarrier> pre_render_barriers;

	std::function<void(VkCommandBuffer)> f_render_cmds{};

	std::vector<ImageBarrier> post_render_barriers;

	static void record(Submission* p_submission);
};