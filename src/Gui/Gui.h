#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include "imgui.h"
#include "memory/Memory.hpp"
#include "memory/Sampler.hpp"
#include "memory/Image.hpp"
#include "memory/Buffer.hpp"

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

	inline Image g_texture;

	inline Memory g_memory{};

	void init(float width, float height);

	ImDrawData* record_frame();
	void process_draw_data(ImDrawData* p_draw_data);
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
	Memory new_memory(Buffer v_buffer, Buffer i_buffer, ImDrawData* p_draw_data);

	inline bool should_create_texture(ImTextureData* p_tex_data)  {
		return p_tex_data->Status == ImTextureStatus_WantCreate && p_tex_data->Pixels;
	}
	inline bool should_update_texture(ImTextureData* p_tex_data) {
		return p_tex_data->Status == ImTextureStatus_WantUpdates && p_tex_data->Pixels;
	}
	Image create_texture_image(ImTextureData* p_tex_data);
	inline void update_texture_image(ImTextureData* p_tex_data, Image texture) {
		// WARNING: texture could be too small to hold p_tex_data.Pixels
		Image::copy_to_image(texture, p_tex_data->Pixels, VkOffset3D{0, 0, 0}, {static_cast<uint32_t>(p_tex_data->Width), static_cast<uint32_t>(p_tex_data->Height), 1}, 0);
	}
	inline void set_status_and_id(ImTextureData* p_tex_data) {
		p_tex_data->SetStatus(ImTextureStatus_OK);
		p_tex_data->SetTexID(reinterpret_cast<intptr_t>(g_texture.image));
	}
}