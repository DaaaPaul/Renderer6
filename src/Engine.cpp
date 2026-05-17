#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <vector>
#include "Engine.h"
#include "FrameKits.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include "MemoryDevice.h"
#include "MemoryHost.h"
#include "Window.h"
#include "Pipelines.h"
#include "PipelineLayouts.h"
#include "OldResources.h"
#include "ImageViewHotspot.h"
#include "Utility.h"
#include "TransformMatrices.hpp"
#include "Camera.hpp"
#include "Vulkan.h"

namespace Engine {
	void record_draw_model(VkCommandBuffer cmd_buf, const uint32_t& sc_image_index) {
		VkImageView sc_image_view = ImageViewHotspot::newView(
			VkImageViewCreateInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = Swapchain::gImages[sc_image_index],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = Swapchain::gIMAGE_FORMAT,
				.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
			}
		);
		VkImageView depth_image_view = ImageViewHotspot::newView(
			VkImageViewCreateInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = Memory::Device::gImages[1].image,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_D32_SFLOAT,
				.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1)
			}
		);
		
		VkRenderingAttachmentInfo sc_image_attachment{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = sc_image_view,
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearColorValue({0.4f, 0.2f, 0.2f, 1.0f}),
		};
		VkRenderingAttachmentInfo depth_image_attachment{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = depth_image_view,
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue = { .depthStencil = VkClearDepthStencilValue(1.0f, 0) },
		};
		VkRenderingInfo rendering_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = VkRect2D(VkOffset2D(0, 0), Swapchain::gStatus.imageExtent),
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &sc_image_attachment,
			.pDepthAttachment = &depth_image_attachment
		};
		Vulkan::begin_cmd_buffer(cmd_buf, VK_NO_FLAGS);

		vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipelines::g_pipelines[0]);
		set_viewport_and_scissor(cmd_buf);

		constexpr VkDeviceSize zero = 0;
		vkCmdBindVertexBuffers(cmd_buf, 0, 1, &Memory::Device::gBuffers[0].buffer, &zero);
		vkCmdBindIndexBuffer(cmd_buf, Memory::Device::gBuffers[1].buffer, 0, VK_INDEX_TYPE_UINT32);

		VkDescriptorSet textureDescriptors = Memory::Device::gDescriptorSets[0].set;
		std::vector<VkDescriptorSet> drawDescriptorSets{ textureDescriptors };
		vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayouts::g_layouts[0], 0, 1, drawDescriptorSets.data(), 0, nullptr);
		
		VkDeviceAddress p_transform_matrices = Memory::Host::gBuffers[3 + FrameKits::g_frame_index].address;
		std::vector<VkDeviceAddress> push_constant{ p_transform_matrices };
		vkCmdPushConstants(cmd_buf, PipelineLayouts::g_layouts[0], VK_SHADER_STAGE_VERTEX_BIT, 0, POINTER_SIZE(1), push_constant.data()); 
		
		Vulkan::insert_image_barrier(cmd_buf, Swapchain::gImages[sc_image_index], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT));

		Vulkan::insert_image_barrier(cmd_buf, Memory::Device::gImages[1].image, VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT));

		vkCmdBeginRendering(cmd_buf, &rendering_info);
		vkCmdDrawIndexed(cmd_buf, Resources::g_model_indices.size(), 1, 0, 0, 0);
		vkCmdEndRendering(cmd_buf);

		VK_CHECK(vkEndCommandBuffer(cmd_buf), "Command buffer end recording failure")
	};

	void render_next() {
		FrameKits::FrameKit& frame_kit = FrameKits::g_frame_kits[FrameKits::g_frame_index];
		frame_kit.progress_sync();

		AcquireStatus acquire = acquire_sc_image(frame_kit.sync_kit.guard);
		if(acquire.result == VK_ERROR_OUT_OF_DATE_KHR || Window::g_window_resized) {
			resize();
			return;
		}
		wait_fence(frame_kit.sync_kit.guard);

		record_draw_model(frame_kit.submit_kits[0].cmd_info.commandBuffer, acquire.sc_image_index);

		std::vector<VkSubmitInfo2> submits{ frame_kit.submit_kits[0].submit_info };
		
		vkQueueSubmit2(LogicalDevice::get_queue(VK_QUEUE_GRAPHICS_BIT), UINT32(submits.size()), submits.data(), VK_NULL_HANDLE);

		wait_timeline_semaphore(frame_kit.sync_kit.timeline_semaphore, frame_kit.sync_pairs[0].signal_val);
		
		if(present_sc_image(acquire.sc_image_index, LogicalDevice::get_queue(VK_QUEUE_GRAPHICS_BIT)) == VK_ERROR_OUT_OF_DATE_KHR || Window::g_window_resized) {
			resize();
			return;
		}

		FrameKits::increment_frame_index();
	}

	void run() {
		std::chrono::steady_clock::time_point before{};
		float delta_time{};

		while(!glfwWindowShouldClose(Window::g_glfw_window)) {
			glfwPollEvents();
			check_close();

			update(delta_time);

			before = std::chrono::high_resolution_clock::now();
			render_next();
			delta_time = get_delta_time(std::chrono::high_resolution_clock::now(), before);
			PRINTLN(delta_time);
		}

		VK_CHECK(vkDeviceWaitIdle(LogicalDevice::g_device), "Failed to wait idle");
	}

	AcquireStatus acquire_sc_image(VkFence fence_to_signal) {
		uint32_t sc_image_index{};
		VkResult result = vkAcquireNextImageKHR(g_device, Swapchain::g_swapchain, UINT64_MAX, VK_NULL_HANDLE, fence_to_signal, &sc_image_index);

		return { sc_image_index, result };
	}

	VkResult present_sc_image(uint32_t index, VkQueue queue) {
		VkPresentInfoKHR present_info{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.swapchainCount = 1,
			.pSwapchains = &Swapchain::g_swapchain,
			.pImageIndices = &index
		};

		return vkQueuePresentKHR(queue, &present_info);
	}

	void wait_timeline_semaphore(VkSemaphore timeline, uint64_t wait_val) {
		VkSemaphoreWaitInfo wait{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
			.semaphoreCount = 1,
			.pSemaphores = &timeline,
			.pValues = &wait_val
		};
		VK_CHECK(vkWaitSemaphores(g_device, &wait, UINT64_MAX), "Failed to wait for semaphore");
	}

	void wait_fence(VkFence fence) {
		VK_CHECK(vkWaitForFences(g_device, 1, &fence, VK_TRUE, UINT64_MAX), "Failed to wait for fence");
		VK_CHECK(vkResetFences(g_device, 1, &fence), "Failed to reset fence");
	}

	void set_viewport_and_scissor(VkCommandBuffer cmd_buf) {
		VkViewport viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(Swapchain::gStatus.imageExtent.width),
			.height = static_cast<float>(Swapchain::gStatus.imageExtent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};
		VkRect2D scissor{
			.offset = VkOffset2D(0, 0),
			.extent = Swapchain::gStatus.imageExtent
		};
		vkCmdSetViewport(cmd_buf, 0, 1, &viewport);
		vkCmdSetScissor(cmd_buf, 0, 1, &scissor);
	}

	void resize() {
		vkDeviceWaitIdle(g_device);

		Swapchain::recreate();
		// TODO: add functionality to recreate depth resources
		Window::g_window_resized = false;
		FrameKits::recreate();
	}

	void check_close() {
		if(glfwGetKey(Window::g_glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(Window::g_glfw_window, GLFW_TRUE);
		}
	}

	void update(float delta_time) {
		g_camera.update_position(delta_time);

		TransformMatrices transformation(glm::mat4(1.0f), Camera::to_view_matrix(g_camera), Camera::to_projection_matrix(g_camera));

		Memory::Host::Mutate::writeToBuffer(3 + FrameKits::g_frame_index, &transformation, sizeof(transformation));
	}

	float get_delta_time(const std::chrono::steady_clock::time_point& time2, const std::chrono::steady_clock::time_point& time1) {
		return std::chrono::duration<float, std::chrono::seconds::period>(time2 - time1).count();
	}
}
