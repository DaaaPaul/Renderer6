#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "memory/ImageView.hpp"
#include "memory/Image.hpp"
#include "memory/Buffer.hpp"

namespace Gui {
	struct PushConstantBlock {
		glm::vec2 scale{};
		glm::vec2 translate{};
	};

	inline ImGuiStyle g_style{};
	inline Buffer g_vertex_buffer;
	inline Buffer g_index_buffer;

	inline Image g_texture;
	inline ImageView g_texture_view;

	void init(float width, float height);
	void destroy();

	ImDrawData* record_frame();
	void process_draw_data(const ImDrawData* p_draw_data);
	inline void check_draw_data(const ImDrawData* p_draw_data) {
		if(!(p_draw_data && p_draw_data->CmdLists.size() > 0)) {
			throw std::runtime_error("check_draw_data: p_draw_data && p_draw_data->CmdLists.size() > 0 is false");
		}
	}
	Buffer fit_vertex_buffer(const ImDrawData* p_draw_data);
	Buffer fit_index_buffer(const ImDrawData* p_draw_data);
	void copy_vertices(const ImDrawData* p_draw_data, Buffer* p_vertex_buffer);
	void copy_indices(const ImDrawData* p_draw_data, Buffer* p_index_buffer);
}