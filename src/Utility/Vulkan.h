#pragma once

#include <vulkan/vulkan_core.h>
#include <ktx.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include "Backend/Instance.h"
#include "Geometry/Vertex.hpp"

#define UINT32(vector_size) static_cast<uint32_t>(vector_size)

#define VK_CHECK(create_command, error_message) \
	if(create_command != VK_SUCCESS) { \
        throw std::runtime_error(error_message); \
	}

#define VK_NO_FLAGS 0U

namespace Vulkan {
	inline PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT{};
	inline PFN_vkCopyMemoryToImageEXT vkCopyMemoryToImageEXT{};

	inline void load() {
		vkTransitionImageLayoutEXT = reinterpret_cast<PFN_vkTransitionImageLayoutEXT>(vkGetInstanceProcAddr(Instance::g_instance, "vkTransitionImageLayoutEXT"));
		vkCopyMemoryToImageEXT = reinterpret_cast<PFN_vkCopyMemoryToImageEXT>(vkGetInstanceProcAddr(Instance::g_instance, "vkCopyMemoryToImageEXT"));
	}

	void load_gltf_model(const char* file_path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

	ktxTexture2* load_ktx_texture(const char* ktx_path);

	void insert_image_barrier(VkCommandBuffer cmd_buf, VkImage image, VkImageSubresourceRange subresource_range, VkPipelineStageFlags2 stage1, VkAccessFlags2 access1, VkPipelineStageFlags2 stage2, VkAccessFlags2 access2, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t graphics_queue_family_index);

	VkResult begin_one_time_cmd_buffer(VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer, uint32_t qfIndex);

	void end_one_time_cmd_buffer(VkQueue queue, VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer);

	VkCommandPool create_cmd_pool(VkCommandPoolCreateFlags flags, uint32_t qf_index);

	VkCommandBuffer create_cmd_buffer(VkCommandPool cmd_pool);

	VkResult begin_cmd_buffer(VkCommandBuffer cmd_buf, VkCommandBufferUsageFlags flags);

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
}