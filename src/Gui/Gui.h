#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include "imgui.h"
#include "Memory/Sampler.hpp"
#include "Memory/Image.hpp"
#include "Memory/Buffer.hpp"

namespace Gui {
	struct PushConstantBlock {
		glm::vec2 scale{};
		glm::vec2 translate{};
	};

	inline ImGuiStyle g_style{};
	inline Buffer g_vertex_buffer;
	inline VkDeviceSize g_vertex_buffer_size = 0;
	inline Buffer g_index_buffer;
	inline VkDeviceSize g_index_buffer_size = 0;

	void init(float width, float height);

	ImDrawData* record_frame();

	inline bool new_draw_data(ImDrawData* p_draw_data) {
		return p_draw_data && p_draw_data->CmdLists.size() > 0;
	}

	inline bool vertex_buffer_too_small(ImDrawData* p_draw_data) {
		return p_draw_data->TotalVtxCount * sizeof(ImDrawVert) > g_vertex_buffer_size;
	}

	inline bool index_buffer_too_small(ImDrawData* p_draw_data) {
		return p_draw_data->TotalIdxCount * sizeof(ImDrawIdx) > g_index_buffer_size;
	}

	Buffer get_vertex_buffer(ImDrawData* p_draw_data);
	Buffer get_index_buffer(ImDrawData* p_draw_data);

	inline bool should_create_texture(ImTextureData* p_tex_data)  {
		return p_tex_data->Status == ImTextureStatus_WantCreate && p_tex_data->Pixels;
	}
	Image create_texture_image(ImTextureData* p_tex_data);
}