#include "Gui.h"
#include "utility/Vulkan.h"
#include "backend/PhysicalDevice.h"

namespace Gui {
	void init(float width, float height) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;

		io.DisplaySize = ImVec2(width, height);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		g_style = ImGui::GetStyle();
		g_style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
		g_style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
		g_style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		g_style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
		g_style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	}

	ImDrawData* record_frame() {
		ImGui::NewFrame();

		ImGui::Begin("Fk you, and you, and you and you and you");
		ImGui::Text("Hello imGui Peter");
		if(ImGui::Button("Click moi")) {
			Utility::println("clicked bi");
		}
		ImGui::End();

		ImGui::Render();

		return ImGui::GetDrawData();
	}

	void process_draw_data(ImDrawData* p_draw_data) {
		if(!new_draw_data(p_draw_data)) {
			throw std::runtime_error("process_draw_data: !new_draw_data(p_draw_data)");
		}

		if(vertex_buffer_too_small(p_draw_data) && index_buffer_too_small(p_draw_data)) {
			g_vertex_buffer.destroy();
			g_vertex_buffer = get_vertex_buffer(p_draw_data);
			g_index_buffer.destroy();
			g_index_buffer = get_index_buffer(p_draw_data);

			g_memory.destroy();
			g_memory = new_memory(g_vertex_buffer, g_index_buffer, p_draw_data);
		} else if(vertex_buffer_too_small(p_draw_data)) {
			g_vertex_buffer.destroy();
			g_vertex_buffer = get_vertex_buffer(p_draw_data);

			g_memory.destroy();
			g_memory = new_memory(g_vertex_buffer, g_index_buffer, p_draw_data);
		} else if(index_buffer_too_small(p_draw_data)) {
			g_index_buffer.destroy();
			g_index_buffer = get_index_buffer(p_draw_data);

			g_memory.destroy();
			g_memory = new_memory(g_vertex_buffer, g_index_buffer, p_draw_data);
		}

		if(p_draw_data->Textures) {
			for(ImTextureData* p_tex_data : *p_draw_data->Textures) {
				if(should_create_texture(p_tex_data)) {
					g_texture = create_texture_image(p_tex_data);
					set_status_and_id(p_tex_data);
				} else if(should_update_texture(p_tex_data)) {
					update_texture_image(p_tex_data, g_texture);
					set_status_and_id(p_tex_data);
				}
			}
		}
	}

	Memory new_memory(Buffer v_buffer, Buffer i_buffer, ImDrawData* p_draw_data) {
		Memory memory({g_vertex_buffer, g_index_buffer}, {}, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, nullptr);

		uint32_t v_offset = 0;
		uint32_t v_size = Utility::INVALID_UINT32;
		uint32_t i_offset = 0;
		uint32_t i_size = Utility::INVALID_UINT32;
		for(ImDrawList* p_cmd_list : p_draw_data->CmdLists) {
			v_size = p_cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
			i_size = p_cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);

			memory.copy_to_buffer(p_cmd_list->VtxBuffer.Data, g_vertex_buffer, v_offset, v_size);
			memory.copy_to_buffer(p_cmd_list->IdxBuffer.Data, g_index_buffer, i_offset, i_size);

			v_offset += v_size;
			i_offset += i_size;
		}

		return memory;
	}

	Buffer get_vertex_buffer(ImDrawData* p_draw_data) {
		if(!new_draw_data(p_draw_data)) {
			throw std::runtime_error("fit_vertex_buffer: !new_draw_data(p_draw_data)");
		}

		return Buffer(Vulkan::NO_FLAGS,
			p_draw_data->TotalVtxCount * sizeof(ImDrawVert),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
	}

	Buffer get_index_buffer(ImDrawData* p_draw_data) {
		if(!new_draw_data(p_draw_data)) {
			throw std::runtime_error("fit_vertex_buffer: !new_draw_data(p_draw_data)");
		}

		return Buffer(Vulkan::NO_FLAGS,
			p_draw_data->TotalIdxCount * sizeof(ImDrawIdx),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
	}

	Image create_texture_image(ImTextureData* p_tex_data) {
		if(!should_create_texture(p_tex_data)) {
			throw std::runtime_error("create_texture: !should_create_texture(p_tex_data)");
		}

		VkExtent3D texture_extent{static_cast<uint32_t>(p_tex_data->Width), static_cast<uint32_t>(p_tex_data->Height), 1};

		Image texture(
			Vulkan::NO_FLAGS,
			VK_IMAGE_TYPE_2D,
			(p_tex_data->Format == ImTextureFormat_RGBA32) ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8_UNORM,
			texture_extent,
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_HOST_TRANSFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);

		Image::transition_image_layout(texture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1);
		update_texture_image(p_tex_data, texture);
	}
}