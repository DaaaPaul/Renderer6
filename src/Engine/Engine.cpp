#include <chrono>
#include <cstdint>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <vector>
#include <algorithm>
#include <vulkan/vulkan_core.h>
#include "backend/LogicalDevice.h"
#include "backend/PhysicalDevice.h"
#include "pipeline/PipelineLayouts.h"
#include "pipeline/Pipelines.h"
#include "backend/Swapchain.h"
#include "backend/Window.h"
#include "shader/PBRPushConstantBlock.hpp"
#include "shader/UniformBufferBlock.hpp"
#include "gui/Gui.h"
#include "memory/MemoryManager.h"
#include "memory/Buffer.hpp"
#include "memory/ImageView.hpp"
#include "utility/Utility.h"
#include "utility/Vulkan.h"
#include "CameraComponent.hpp"
#include "Engine.h"
#include "EntityManager.h"
#include "SubmissionManager.h"

namespace Engine {
	void render(Index sc_index) {
		ScAcquire acquire = acquire_sc_image(g_fences[sc_index.val]);
		if(acquire.result == VK_ERROR_OUT_OF_DATE_KHR || Window::g_window_user_pointer->window_resized) {
			resize();
			return;
		}
		wait_fence(g_fences[sc_index.val]);

		std::vector<Submission> submissions{ 
			*SubmissionManager::g_submissions.get(std::string("axe ") + std::to_string(sc_index.val)), 
			*SubmissionManager::g_submissions.get(std::string("sphere ") + std::to_string(sc_index.val)),
			Gui::get_submission(Gui::record_frame(), sc_index.val)
		};
		Submission::record(&submissions[2]);
		
		VkQueue queue = LogicalDevice::get_queue(VK_QUEUE_GRAPHICS_BIT);
		Submission::submit(queue, VK_NULL_HANDLE, &submissions);

		if(present_sc_image(sc_index.val, queue) == VK_ERROR_OUT_OF_DATE_KHR || Window::g_window_user_pointer->window_resized) {
			resize();
			return;
		}
	}

	void run() {
		g_fences.reserve(Swapchain::g_IMAGE_COUNT);
		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			g_fences.push_back(Vulkan::create_fence(Vulkan::NO_FLAGS));
		}

		std::chrono::steady_clock::time_point before{};
		std::chrono::steady_clock::time_point after{};
		float delta_time{};
		g_ubo_data = UniformBufferBlock(*EntityManager::g_camera.get<CameraComponent>());

		Index index(Swapchain::g_IMAGE_COUNT, 0);


		while(!glfwWindowShouldClose(Window::g_glfw_window)) {
			glfwPollEvents();
			check_close();

			update(delta_time);

			before = std::chrono::high_resolution_clock::now();
			render(index);
			after = std::chrono::high_resolution_clock::now();

			delta_time = std::chrono::duration<float, std::chrono::seconds::period>(after - before).count();
			g_total_delta_time += delta_time;
			++g_total_loops;

			index.increase();
		}

		Utility::println(std::to_string(g_total_delta_time / g_total_loops));

		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			vkDestroyFence(g_device, g_fences[i], nullptr);
		}

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

		Buffer::copy_to(MemoryManager::g_buffers.get("uniform buffer 0"), &g_ubo_data, 0, sizeof(g_ubo_data));
	}
}
