#include "Backend/LogicalDevice.h"
#include "Backend/PhysicalDevice.h"
#include "Pipeline/PipelineLayouts.h"
#include "Pipeline/Pipelines.h"
#include "Backend/Swapchain.h"
#include "Backend/Window.h"
#include "CameraComponent.hpp"
#include "Engine.h"
#include "Frame.hpp"
#include "EntityManager.h"
#include "ShaderStructs/PushConstantBlock.hpp"
#include "ShaderStructs/UniformBufferBlock.hpp"
#include "Memory/MemoryManager.h"
#include "Memory/Buffer.hpp"
#include "Memory/ImageView.hpp"
#include "Utility/Ids.h"
#include "Utility/Utility.h"
#include "Utility/Vulkan.h"
#include <chrono>
#include <cstdint>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Engine {
	void record_draw_model(VkCommandBuffer cmd_buf, uint32_t sc_image_index) {		
		VkRenderingAttachmentInfo sc_image_attachment{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = Swapchain::g_image_views[sc_image_index],
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearColorValue({0.4f, 0.2f, 0.2f, 1.0f}),
		};
		VkRenderingAttachmentInfo depth_image_attachment{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = MemoryManager::g_image_views.get<ImageView>(Ids::g_DEPTH_VIEW)->get_image_view(),
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = { .depthStencil = VkClearDepthStencilValue(1.0f, 0) },
		};
		VkRenderingInfo rendering_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = VkRect2D(VkOffset2D(0, 0), Swapchain::g_status.imageExtent),
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &sc_image_attachment,
			.pDepthAttachment = &depth_image_attachment
		};
		Vulkan::begin_cmd_buffer(cmd_buf, Vulkan::NO_FLAGS);

		vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipelines::g_pipelines[0]);
		set_viewport_and_scissor(cmd_buf);

		const std::vector<VkDeviceSize> VERTEX_BUFFER_OFFSETS{ 0 };
		VkBuffer vertex_buffer = MemoryManager::g_buffers.get<Buffer>(Ids::g_VERTEX_BUFFER)->buffer;
		VkBuffer index_buffer = MemoryManager::g_buffers.get<Buffer>(Ids::g_INDEX_BUFFER)->buffer;
		vkCmdBindVertexBuffers(cmd_buf, 0, 1, &vertex_buffer, VERTEX_BUFFER_OFFSETS.data());
		vkCmdBindIndexBuffer(cmd_buf, index_buffer, 0, VK_INDEX_TYPE_UINT32);

		VkDescriptorSet descriptor_set = MemoryManager::g_descriptor_sets.get<DescriptorSet>(Ids::g_DESCRIPTOR_SET)->get_descriptor_set();
		vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayouts::g_layouts[0], 0, 1, &descriptor_set, 0, nullptr);
		
		PushConstantBlock push_constants{
			.base_color_factor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			.metallicFactor = 1.0f,
			.roughnessFactor = 1.0f,
			.baseColorTextureSet = true,
			.physicalDescriptorTextureSet = true,
			.normalTextureSet = true,
		};
		vkCmdPushConstants(cmd_buf, PipelineLayouts::g_layouts[0], VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantBlock), &push_constants); 
		
		Vulkan::insert_image_barrier(cmd_buf, Swapchain::g_images[sc_image_index], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT));

		Vulkan::insert_image_barrier(cmd_buf, MemoryManager::g_images.get<Image>(Ids::g_DEPTH_IMAGE)->image, VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT));

		vkCmdBeginRendering(cmd_buf, &rendering_info);
		vkCmdDrawIndexed(cmd_buf, MemoryManager::g_indices.size(), 1, 0, 0, 0);
		vkCmdEndRendering(cmd_buf);

		Vulkan::check(vkEndCommandBuffer(cmd_buf), "Command buffer end recording failure");
	};

	void record_draw_simple(VkCommandBuffer cmd_buf, uint32_t sc_image_index) {
		VkRenderingAttachmentInfo sc_image_attachment{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = Swapchain::g_image_views[sc_image_index],
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		};
		VkRenderingAttachmentInfo depth_image_attachment{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = MemoryManager::g_image_views.get<ImageView>(Ids::g_DEPTH_VIEW)->get_image_view(),
			.imageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		};
		VkRenderingInfo rendering_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = VkRect2D(VkOffset2D(0, 0), Swapchain::g_status.imageExtent),
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &sc_image_attachment,
			.pDepthAttachment = &depth_image_attachment
		};
		Vulkan::begin_cmd_buffer(cmd_buf, Vulkan::NO_FLAGS);

		vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipelines::g_pipelines[1]);
		set_viewport_and_scissor(cmd_buf);

		const std::vector<VkDeviceSize> VERTEX_BUFFER_OFFSETS{ 0 };
		VkBuffer vertex_buffer = MemoryManager::g_buffers.get<Buffer>(Ids::g_SIMPLE_VERTEX_BUFFER)->buffer;
		VkBuffer index_buffer = MemoryManager::g_buffers.get<Buffer>(Ids::g_SIMPLE_INDEX_BUFFER)->buffer;
		vkCmdBindVertexBuffers(cmd_buf, 0, 1, &vertex_buffer, VERTEX_BUFFER_OFFSETS.data());
		vkCmdBindIndexBuffer(cmd_buf, index_buffer, 0, VK_INDEX_TYPE_UINT32);

		struct SimplePushConstantBlock {
			glm::mat4 model{};
			glm::mat4 view{};
			glm::mat4 proj{};
		};
		CameraComponent* camera_component = EntityManager::g_camera.get<CameraComponent>();
		SimplePushConstantBlock push_constants{
			.model = glm::translate(glm::mat4(1.0f), glm::vec3(g_circle_position.x, 0.0f, g_circle_position.y)),
			.view = CameraComponent::to_view_matrix(*camera_component),
			.proj = CameraComponent::to_projection_matrix(*camera_component),
		};

		vkCmdPushConstants(cmd_buf, PipelineLayouts::g_layouts[1], VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(SimplePushConstantBlock), &push_constants); 

		vkCmdBeginRendering(cmd_buf, &rendering_info);
		vkCmdDrawIndexed(cmd_buf, MemoryManager::g_simple_indices.size(), 1, 0, 0, 0);
		vkCmdEndRendering(cmd_buf);

		Vulkan::insert_image_barrier(cmd_buf, Swapchain::g_images[sc_image_index], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE, 
		VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT));

		Vulkan::check(vkEndCommandBuffer(cmd_buf), "Command buffer end recording failure");
	}

	void render_next(Frame& frame) {
		frame.progress_timeline();

		ScAcquire acquire = acquire_sc_image(frame.fence);
		if(acquire.result == VK_ERROR_OUT_OF_DATE_KHR || Window::g_window_user_pointer->window_resized) {
			resize();
			return;
		}
		wait_fence(frame.fence);

		record_draw_model(frame.submits[0].cmd.commandBuffer, acquire.sc_image_index);
		record_draw_simple(frame.submits[1].cmd.commandBuffer, acquire.sc_image_index);

		std::vector<VkSubmitInfo2> submits{ frame.submits[0].submit, frame.submits[1].submit };
		
		vkQueueSubmit2(LogicalDevice::get_queue(VK_QUEUE_GRAPHICS_BIT), static_cast<uint32_t>(submits.size()), submits.data(), VK_NULL_HANDLE);

		wait_timeline_semaphore(frame.timeline, frame.timeline_val);
		
		if(present_sc_image(acquire.sc_image_index, LogicalDevice::get_queue(VK_QUEUE_GRAPHICS_BIT)) == VK_ERROR_OUT_OF_DATE_KHR || Window::g_window_user_pointer->window_resized) {
			resize();
			return;
		}
	}

	void run() {
		FramesInFlight frames_in_flight(Swapchain::g_IMAGE_COUNT, 2);

		std::chrono::steady_clock::time_point before{};
		std::chrono::steady_clock::time_point after{};
		float delta_time{};
		g_ubo_data = UniformBufferBlock(*EntityManager::g_camera.get<CameraComponent>());

		while(!glfwWindowShouldClose(Window::g_glfw_window)) {
			glfwPollEvents();
			check_close();

			update(delta_time);

			before = std::chrono::high_resolution_clock::now();
			render_next(frames_in_flight.frames[frames_in_flight.frame_index.val]);
			++frames_in_flight.frame_index;
			after = std::chrono::high_resolution_clock::now();

			delta_time = std::chrono::duration<float, std::chrono::seconds::period>(after - before).count();
			g_total_delta_time += delta_time;
			++g_total_loops;
		}

		Utility::println(std::to_string(g_total_delta_time / g_total_loops));

		Vulkan::check(vkDeviceWaitIdle(LogicalDevice::g_device), "Failed to wait idle");
	}

	ScAcquire acquire_sc_image(VkFence fence_to_signal) {
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
		Vulkan::check(vkWaitSemaphores(g_device, &wait, UINT64_MAX), "Failed to wait for semaphore");
	}

	void wait_fence(VkFence fence) {
		Vulkan::check(vkWaitForFences(g_device, 1, &fence, VK_TRUE, UINT64_MAX), "Failed to wait for fence");
		Vulkan::check(vkResetFences(g_device, 1, &fence), "Failed to reset fence");
	}

	void set_viewport_and_scissor(VkCommandBuffer cmd_buf) {
		VkViewport viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(Swapchain::g_status.imageExtent.width),
			.height = static_cast<float>(Swapchain::g_status.imageExtent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};
		VkRect2D scissor{
			.offset = VkOffset2D(0, 0),
			.extent = Swapchain::g_status.imageExtent
		};
		vkCmdSetViewport(cmd_buf, 0, 1, &viewport);
		vkCmdSetScissor(cmd_buf, 0, 1, &scissor);
	}

	void resize() {
		vkDeviceWaitIdle(g_device);

		Swapchain::recreate();
		// TODO: add functionality to recreate depth resources
		Window::g_window_user_pointer->window_resized = false;
		//Frame::recreate();
	}

	void check_close() {
		if(glfwGetKey(Window::g_glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(Window::g_glfw_window, GLFW_TRUE);
		}
	}

	void update(float delta_time) {
		CameraComponent* camera_component = EntityManager::g_camera.get<CameraComponent>();
		camera_component->set_position(CameraComponent::process_position(camera_component->get_basis(), camera_component->get_position(), delta_time));
		
		g_ubo_data.update(*camera_component);

		g_circle_position = Utility::get_circle_position(g_total_delta_time, 3.0f);
		g_ubo_data.light_positions[0].x = g_circle_position.x;
		g_ubo_data.light_positions[0].z = g_circle_position.y;

		MemoryManager::g_host_memory.copy_to_buffer(&g_ubo_data, sizeof(UniformBufferBlock), *MemoryManager::g_buffers.get<Buffer>(Ids::g_UNIFORM_BUFFERS[0]));
	}
}
