#pragma once

#include <vulkan/vulkan_core.h>
#include <vma/vk_mem_alloc.h>
#include <ktx.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include "backend/Instance.h"
#include "shader/PBRVertex.hpp"
#include "backend/Swapchain.h"
#include "UtilityStructs.hpp"

namespace Vulkan {
	inline constexpr VkFlags NO_FLAGS = 0U;

	inline void check(VkResult result, const char* error_msg = "Vulkan::check: failed") {
		if(result != VK_SUCCESS) {
			throw std::runtime_error(error_msg);
		}
	}

	inline PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT{};
	inline PFN_vkCopyMemoryToImageEXT vkCopyMemoryToImageEXT{};
	inline PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr{};

	inline VmaAllocator g_vma_allocator{};
	
	void load_functions();
	void init_vma();
	inline void destroy_vma() {
		vmaDestroyAllocator(g_vma_allocator);
	}

	std::pair<std::vector<PBRVertex>, std::vector<uint32_t>> load_gltf_model(const std::string& file_path);

	ktxTexture2* load_ktx_texture(const char* ktx_path, ktx_transcode_fmt_e transcode_format);

	void transcode_ktx_texture(ktxTexture2* ktx_texture, ktx_transcode_fmt_e target_format);

	void begin_one_time_cmd_buffer(VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer, uint32_t qfIndex);

	void end_one_time_cmd_buffer(VkQueue queue, VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer);

	VkCommandPool create_cmd_pool(VkCommandPoolCreateFlags flags, uint32_t qf_index);

	VkCommandBuffer create_cmd_buffer(VkCommandPool cmd_pool);

	void begin_cmd_buffer(VkCommandBuffer cmd_buf, VkCommandBufferUsageFlags flags);

	inline void end_cmd_buffer(VkCommandBuffer cmd_buf) {
		Vulkan::check(vkEndCommandBuffer(cmd_buf), "end_cmd_buffer: failed");
	}

	VkFence create_fence(VkFenceCreateFlags flags);

	VkSemaphore create_semaphore(VkSemaphoreType semaphore_type);

	VkDeviceMemory create_memory(VkDeviceSize size, uint32_t type_index, const void* p_next);

	VkBuffer create_buffer(VkDeviceSize size, VkBufferUsageFlags usage);

	VkPipelineLayout create_pipeline_layout(const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const std::vector<VkPushConstantRange>& push_constant_ranges);

	VkShaderModule create_shader_module(const std::string& file_path);

	VkSurfaceKHR create_surface();

	VkDescriptorSetLayout create_descriptor_set_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings, const std::vector<VkDescriptorBindingFlags>& binding_flags = {});

	VkDescriptorPool create_descriptor_pool(const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& all_bindings);

	VkDescriptorSet create_descriptor_set(VkDescriptorSetLayout layout, VkDescriptorPool pool);

	std::vector<VkImage> get_swapchain_images(VkSwapchainKHR swapchain);

	std::vector<VkImageView> get_image_views(const std::vector<VkImage>& images, VkImageViewType view_type, VkImageAspectFlags image_aspect, VkFormat format);

	inline VkViewport get_sc_viewport() {
		return {
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(Swapchain::g_status.imageExtent.width),
			.height = static_cast<float>(Swapchain::g_status.imageExtent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

	}

	inline VkRect2D get_sc_scissor() {
		return {
			.offset = VkOffset2D(0, 0),
			.extent = Swapchain::g_status.imageExtent
		};
	}
}