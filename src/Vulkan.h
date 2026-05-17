#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace Vulkan {
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
}