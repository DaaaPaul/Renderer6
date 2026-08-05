#include "Gui.h"
#include "utility/Vulkan.h"
#include "utility/Utility.h"
#include "backend/PhysicalDevice.h"
#include "memory/MemoryManager.h"

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

	void destroy() {
		ImGui::DestroyContext();
		g_texture_view.destroy();
		g_texture.destroy();
		g_vertex_buffer.destroy();
		g_index_buffer.destroy();
	}

	ImDrawData* record_frame() {
		ImGui::NewFrame();

		ImGui::Begin("Paul Paul");
		ImGui::Text("Hello imGui Peter");
		if(ImGui::Button("Click moi")) {
			Utility::println("clicked bi");
		}
		ImGui::End();

		ImGui::Render();

		return ImGui::GetDrawData();
	}

	void process_draw_data(const ImDrawData* p_draw_data) {
		check_draw_data(p_draw_data);

		if(Buffer::smaller(&g_vertex_buffer, sizeof(ImDrawVert), p_draw_data->TotalVtxCount)) {
			g_vertex_buffer.destroy();
			g_vertex_buffer = fit_vertex_buffer(p_draw_data);
			copy_vertices(p_draw_data, &g_vertex_buffer);
		}
		if(Buffer::smaller(&g_index_buffer, sizeof(ImDrawIdx), p_draw_data->TotalIdxCount)) {
			g_index_buffer.destroy();
			g_index_buffer = fit_index_buffer(p_draw_data);
			copy_indices(p_draw_data, &g_index_buffer);
		}

		if(p_draw_data->Textures) {
			for(ImTextureData* p_tex_data : *p_draw_data->Textures) {
				if(p_tex_data->Pixels) {
					if(p_tex_data->Status == ImTextureStatus_WantCreate) {
						g_texture_data = *p_tex_data;

						g_texture.destroy();
						g_texture = Image(
							Vulkan::NO_FLAGS,
							VK_IMAGE_TYPE_2D,
							(p_tex_data->Format == ImTextureFormat_RGBA32) ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8_UNORM,
							VkExtent3D{static_cast<uint32_t>(p_tex_data->Width), static_cast<uint32_t>(p_tex_data->Height), 1},
							1,
							1,
							VK_SAMPLE_COUNT_1_BIT,
							VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_HOST_TRANSFER_BIT,
							VK_SHARING_MODE_EXCLUSIVE,
							PhysicalDevice::g_graphics_family_index,
							VmaAllocationCreateInfo{
								.usage = VMA_MEMORY_USAGE_AUTO
							}
						);

						// WARNING: texture could be too small to hold p_tex_data.Pixels
						Image::transition_layout(g_texture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1);
						Image::copy_to(g_texture, p_tex_data->Pixels, VkOffset3D{0, 0, 0}, {static_cast<uint32_t>(p_tex_data->Width), static_cast<uint32_t>(p_tex_data->Height), 1}, 0);

						g_texture_view.destroy();
						g_texture_view = ImageView(
							Vulkan::NO_FLAGS, 
							g_texture.get_image(), 
							VK_IMAGE_VIEW_TYPE_2D, 
							(p_tex_data->Format == ImTextureFormat_RGBA32) ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8_UNORM,
							VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
						);

						p_tex_data->SetStatus(ImTextureStatus_OK);
						p_tex_data->SetTexID(reinterpret_cast<intptr_t>(g_texture.get_image()));
					} else if(p_tex_data->Status == ImTextureStatus_WantUpdates) {
						g_texture_data = *p_tex_data;

						Image::copy_to(g_texture, p_tex_data->Pixels, VkOffset3D{0, 0, 0}, {static_cast<uint32_t>(p_tex_data->Width), static_cast<uint32_t>(p_tex_data->Height), 1}, 0);
						p_tex_data->SetStatus(ImTextureStatus_OK);
						p_tex_data->SetTexID(reinterpret_cast<intptr_t>(g_texture.get_image()));
					}
				}
			}
		}
	}

	Buffer fit_vertex_buffer(const ImDrawData* p_draw_data) {
		check_draw_data(p_draw_data);

		return Buffer(Vulkan::NO_FLAGS,
			p_draw_data->TotalVtxCount * sizeof(ImDrawVert),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			VmaAllocationCreateInfo{
				.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
				.usage = VMA_MEMORY_USAGE_AUTO
			}
		);
	}

	Buffer fit_index_buffer(const ImDrawData* p_draw_data) {
		check_draw_data(p_draw_data);

		return Buffer(Vulkan::NO_FLAGS,
			p_draw_data->TotalIdxCount * sizeof(ImDrawIdx),
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			VmaAllocationCreateInfo{
				.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
				.usage = VMA_MEMORY_USAGE_AUTO
			}
		);
	}

	void copy_vertices(const ImDrawData* p_draw_data, Buffer* p_vertex_buffer) {
		uint32_t offset = 0;
		uint32_t size = Utility::INVALID_UINT32;

		for(const ImDrawList* p_cmd_list : p_draw_data->CmdLists) {
			size = p_cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);

			Buffer::copy_to(p_vertex_buffer, p_cmd_list->VtxBuffer.Data, offset, size);
			offset += size;
		}
	}

	void copy_indices(const ImDrawData* p_draw_data, Buffer* p_index_buffer) {
		uint32_t offset = 0;
		uint32_t size = Utility::INVALID_UINT32;

		for(const ImDrawList* p_cmd_list : p_draw_data->CmdLists) {
			size = p_cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);

			Buffer::copy_to(p_index_buffer, p_cmd_list->IdxBuffer.Data, offset, size);
			offset += size;
		}
	}
}