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

		return Image(
			Vulkan::NO_FLAGS,
			VK_IMAGE_TYPE_2D,
			(p_tex_data->Format == ImTextureFormat_RGBA32) ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8_UNORM,
			VkExtent3D{static_cast<uint32_t>(p_tex_data->Width), static_cast<uint32_t>(p_tex_data->Height), 1},
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_HOST_TRANSFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
	}
}