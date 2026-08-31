#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"
#include "backend/Window.h"
#include "pipeline/Submission.hpp"
#include "utility/Vulkan.h"

namespace Gui {
	class FrameRecorded {
		FrameRecorded() = default;

		friend FrameRecorded record_frame();
	};

	inline ImGui_ImplVulkan_InitInfo g_vulkan_init_info{};
	inline VkFormat g_color_attachment_format{};
	inline VkDescriptorPool g_descriptor_pool{};
	inline VkCommandPool g_cmd_pool{};

	void init(ImGuiConfigFlags config_flags, ImGuiBackendFlags backend_flags);
	void destroy();

	inline void set_cmd_pool() {
		g_cmd_pool = Vulkan::create_cmd_pool(Vulkan::NO_FLAGS, PhysicalDevice::g_graphics_family_index[0]);
	}

	inline void destroy_cmd_pool() {
		vkDestroyCommandPool(g_device, g_cmd_pool, nullptr);
	}

	inline void create_context() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
	}

	inline void destroy_context() {
		ImGui::DestroyContext();
	}

	void set_io_context(ImGuiConfigFlags config_flags, ImGuiBackendFlags backend_flags);

	inline void set_imgui_glfw_window() {
		ImGui_ImplGlfw_InitForVulkan(Window::g_glfw_window, true);
	}

	inline void shutdown_imgui_glfw_window() {
		ImGui_ImplGlfw_Shutdown();
	}

	void set_descriptor_pool();

	inline void destroy_descriptor_pool() {
		vkDestroyDescriptorPool(g_device, g_descriptor_pool, nullptr);
	}

	void set_vulkan_init_info();

	inline ImGui_ImplVulkan_InitInfo* get_vulkan_init_info() {
		return &g_vulkan_init_info;
	}

	inline void set_imgui_vulkan_application() {
		ImGui_ImplVulkan_Init(&g_vulkan_init_info);
	}

	inline void shutdown_imgui_vulkan_application() {
		ImGui_ImplVulkan_Shutdown();
	}

	FrameRecorded record_frame();

	Submission get_submission(FrameRecorded frame_recorded, uint32_t sc_image_index);

	void imgui_check(VkResult result);
}