#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"
#include "backend/Instance.h"

namespace Gui {
	inline ImGui_ImplVulkan_InitInfo g_vulkan_init_info{};

	inline void create_context() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
	}

	void set_io_context(glm::vec2 display_size, ImGuiConfigFlags config_flags, ImGuiBackendFlags backend_flags);

	inline void sync_glfw_callbacks(GLFWwindow* p_glfw_window) {
		ImGui_ImplGlfw_InitForVulkan(p_glfw_window, true);
	}

	inline void set_vulkan_init_info() {
		g_vulkan_init_info.ApiVersion = Instance::
	}

	inline ImGui_ImplVulkan_InitInfo* get_vulkan_init_info() {
		return &g_vulkan_init_info;
	}
}