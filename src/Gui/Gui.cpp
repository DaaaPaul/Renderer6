#include "Gui.h"
#include "backend/Instance.h"
#include "backend/PhysicalDevice.h"
#include "backend/LogicalDevice.h"
#include "backend/Swapchain.h"

namespace Gui {
	void init(ImGuiConfigFlags config_flags, ImGuiBackendFlags backend_flags) {
		set_cmd_pool();
		create_context();
		set_io_context(config_flags, backend_flags);
		set_imgui_glfw_window();
		set_descriptor_pool();
		set_vulkan_init_info();
		set_imgui_vulkan_application();
	}

	void destroy() {
		destroy_cmd_pool();
		shutdown_imgui_vulkan_application();
		destroy_descriptor_pool();
		shutdown_imgui_glfw_window();
		destroy_context();
	}

	void set_io_context(ImGuiConfigFlags config_flags, ImGuiBackendFlags backend_flags) {
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = static_cast<float>(Swapchain::g_status.imageExtent.width);
		io.DisplaySize.y = static_cast<float>(Swapchain::g_status.imageExtent.height);
		io.ConfigFlags = config_flags;
		io.BackendFlags = backend_flags;
	}

	void set_descriptor_pool() {
		std::vector<VkDescriptorPoolSize> pool_sizes{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo descriptor_pool_create = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets = static_cast<uint32_t>(1000 * pool_sizes.size()),
			.poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
			.pPoolSizes = pool_sizes.data()
		};

		Vulkan::check(vkCreateDescriptorPool(g_device, &descriptor_pool_create, nullptr, &g_descriptor_pool), "set_descriptor_pool: failed");
	}

	void set_vulkan_init_info() {
		g_color_attachment_format = Swapchain::g_IMAGE_FORMAT;

		g_vulkan_init_info.ApiVersion = Instance::get_create_info()->pApplicationInfo->apiVersion;
		g_vulkan_init_info.Instance = Instance::get_instance();
		g_vulkan_init_info.PhysicalDevice = PhysicalDevice::g_physical_device;
		g_vulkan_init_info.Device = g_device;
		g_vulkan_init_info.QueueFamily = PhysicalDevice::g_graphics_family_index[0];
		g_vulkan_init_info.Queue = LogicalDevice::get_queue(VK_QUEUE_GRAPHICS_BIT);
		g_vulkan_init_info.DescriptorPool = g_descriptor_pool;
		g_vulkan_init_info.MinImageCount = Swapchain::g_surface_capabilities.minImageCount;
		g_vulkan_init_info.ImageCount = Swapchain::g_IMAGE_COUNT;
		g_vulkan_init_info.PipelineInfoMain = ImGui_ImplVulkan_PipelineInfo{
			.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
			.PipelineRenderingCreateInfo = VkPipelineRenderingCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
				.colorAttachmentCount = 1,
				.pColorAttachmentFormats = &g_color_attachment_format,
				.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT
			}
		};
		g_vulkan_init_info.UseDynamicRendering = true;
		g_vulkan_init_info.CheckVkResultFn = &imgui_check;
	}

	FrameRecorded record_frame() {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		ImGui::ShowDemoWindow();

		ImGui::Render();

		return {};
	}

	Submission get_submission(FrameRecorded frame_recorded, uint32_t sc_image_index) {
		VkCommandBuffer cmd_buf = Vulkan::create_cmd_buffer(g_cmd_pool);
		
		return Submission{
			cmd_buf,
			sc_image_index,
			std::function<void(VkCommandBuffer)>{},
			std::function<void(VkCommandBuffer)>{},
			std::vector<ImageBarrier>{},
			[](VkCommandBuffer cmd_buf) -> void {
				ImDrawData* p_draw_data = ImGui::GetDrawData();
				ImGui_ImplVulkan_RenderDrawData(p_draw_data, cmd_buf);
			},
			std::vector<ImageBarrier>{
				ImageBarrier{
					.cmd_buf = cmd_buf,
					.image = Swapchain::g_images[sc_image_index],
					.image_subresource_range{
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.baseMipLevel = 0,
						.levelCount = 1,
						.baseArrayLayer = 0,
						.layerCount = 1
					},
					.sync_stage_1 = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
					.access_stage_1 = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
					.sync_stage_2 = VK_PIPELINE_STAGE_2_NONE,
					.access_stage_2 = VK_ACCESS_2_NONE,
					.old_layout = VK_IMAGE_LAYOUT_GENERAL,
					.new_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				}
			}
		};
	}

	void imgui_check(VkResult result) {
		char result_char = result + 42;

		Vulkan::check(result, &result_char);
	}
}