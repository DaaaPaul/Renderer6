#include "SubmissionManager.h"
#include "shader/PBRPushConstantBlock.hpp"
#include "shader/SimplePushConstantBlock.hpp"
#include "pipeline/PipelineLayouts.h"
#include "pipeline/Pipelines.h"
#include "memory/MemoryManager.h"
#include "EntityManager.h"
#include "Engine.h"

namespace SubmissionManager {
	void init() {
		set_cmd_pool();
		set_submissions();
		record_submissions();
	}

	void destroy() {
		destroy_submissions();
		destroy_cmd_pool();
	}

	void set_submissions() {
		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			VkCommandBuffer axe_i_cmd_buf = Vulkan::create_cmd_buffer(g_cmd_pool);

			g_submissions.add(
				std::string("axe ") + std::to_string(i), 
				axe_i_cmd_buf,
				i,
				0.4f, 
				0.2f, 
				0.2f, 
				1.0f,
				VkClearDepthStencilValue{ .depth = 1.0f },
				[](VkCommandBuffer cmd_buf) -> void {
					set_sc_viewport(cmd_buf);
					set_sc_scissor(cmd_buf);

					PBRPushConstantBlock push_constants{
						.base_color_factor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
						.metallicFactor = 1.0f,
						.roughnessFactor = 1.0f,
					};
					vkCmdPushConstants(cmd_buf, PipelineLayouts::g_layouts[0], VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PBRPushConstantBlock), &push_constants); 
				},
				[](VkCommandBuffer cmd_buf) -> void {
					vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipelines::g_pipelines[0]);

					std::vector<VkDeviceSize> vertex_buffer_offsets{ 0 };
					std::vector<VkBuffer> vertex_buffers{ MemoryManager::g_buffers.get("vertex buffer")->get_buffer() };
					vkCmdBindVertexBuffers(cmd_buf, 0, 1, vertex_buffers.data(), vertex_buffer_offsets.data());
					vkCmdBindIndexBuffer(cmd_buf, MemoryManager::g_buffers.get("index buffer")->get_buffer(), 0, VK_INDEX_TYPE_UINT32);

					VkDescriptorSet descriptor_set = MemoryManager::g_descriptor_sets.get("descriptor set")->get_descriptor_set();
					vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayouts::g_layouts[0], 0, 1, &descriptor_set, 0, nullptr);
				},
				std::vector<ImageBarrier>{
					ImageBarrier{
						.cmd_buf = axe_i_cmd_buf,
						.image = Swapchain::g_images[i],
						.image_subresource_range{
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.baseMipLevel = 0,
							.levelCount = 1,
							.baseArrayLayer = 0,
							.layerCount = 1
						},
						.sync_stage_1 = VK_PIPELINE_STAGE_2_NONE,
						.access_stage_1 = VK_ACCESS_2_NONE,
						.sync_stage_2 = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
						.access_stage_2 = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
						.old_layout = VK_IMAGE_LAYOUT_UNDEFINED,
						.new_layout = VK_IMAGE_LAYOUT_GENERAL
					},
					ImageBarrier{
						.cmd_buf = axe_i_cmd_buf,
						.image = MemoryManager::g_images.get("depth image")->get_image(),
						.image_subresource_range{
							.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
							.baseMipLevel = 0,
							.levelCount = 1,
							.baseArrayLayer = 0,
							.layerCount = 1
						},
						.sync_stage_1 = VK_PIPELINE_STAGE_2_NONE,
						.access_stage_1 = VK_ACCESS_2_NONE,
						.sync_stage_2 = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
						.access_stage_2 = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
						.old_layout = VK_IMAGE_LAYOUT_UNDEFINED,
						.new_layout = VK_IMAGE_LAYOUT_GENERAL
					}
				},
				[](VkCommandBuffer cmd_buf) -> void {
					vkCmdDrawIndexed(cmd_buf, MemoryManager::g_indices.size(), 1, 0, 0, 0);
				},
				std::vector<ImageBarrier>{}
			);

			VkCommandBuffer sphere_i_cmd_buf = Vulkan::create_cmd_buffer(g_cmd_pool);

			g_submissions.add(
				std::string("sphere ") + std::to_string(i), 
				sphere_i_cmd_buf,
				i,
				[](VkCommandBuffer cmd_buf) -> void {
					set_sc_viewport(cmd_buf);
					set_sc_scissor(cmd_buf);

					CameraComponent* p_camera_component = EntityManager::g_camera.get<CameraComponent>();
					SimplePushConstantBlock push_constants{
						.model = glm::translate(glm::mat4(1.0f), glm::vec3(Engine::g_circle_position.x, 0.0f, Engine::g_circle_position.y)),
						.view = p_camera_component->view_matrix(),
						.proj = p_camera_component->projection_matrix(),
					};

					vkCmdPushConstants(cmd_buf, PipelineLayouts::g_layouts[1], VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(SimplePushConstantBlock), &push_constants); 
				},
				[](VkCommandBuffer cmd_buf) -> void {
					vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipelines::g_pipelines[1]);

					std::vector<VkDeviceSize> vertex_buffer_offsets{ 0 };
					std::vector<VkBuffer> vertex_buffers{ MemoryManager::g_buffers.get("sphere vertex buffer")->get_buffer() };
					vkCmdBindVertexBuffers(cmd_buf, 0, 1, vertex_buffers.data(), vertex_buffer_offsets.data());
					vkCmdBindIndexBuffer(cmd_buf, MemoryManager::g_buffers.get("sphere index buffer")->get_buffer(), 0, VK_INDEX_TYPE_UINT32);
				},
				std::vector<ImageBarrier>{},
				[](VkCommandBuffer cmd_buf) -> void {
					vkCmdDrawIndexed(cmd_buf, MemoryManager::g_sphere_indices.size(), 1, 0, 0, 0);
				},
				std::vector<ImageBarrier>{}
			);
		}
	}
	
	void record_submissions() {
		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			Submission::record(g_submissions.get(std::string("axe ") + std::to_string(i)));
			Submission::record(g_submissions.get(std::string("sphere ") + std::to_string(i)));
		}
	}

	void destroy_submissions() {
		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			vkFreeCommandBuffers(g_device, g_cmd_pool, 1, &g_submissions.get(std::string("axe ") + std::to_string(i))->cmd_buf);
			vkFreeCommandBuffers(g_device, g_cmd_pool, 1, &g_submissions.get(std::string("sphere ") + std::to_string(i))->cmd_buf);
		}
	}

	void set_sc_viewport(VkCommandBuffer cmd_buf) {
		VkViewport viewport = Vulkan::get_sc_viewport();
		vkCmdSetViewport(cmd_buf, 0, 1, &viewport);
	}

	void set_sc_scissor(VkCommandBuffer cmd_buf) {
		VkRect2D scissor = Vulkan::get_sc_scissor();
		vkCmdSetScissor(cmd_buf, 0, 1, &scissor);
	}
}