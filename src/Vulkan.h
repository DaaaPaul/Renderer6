#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace Vulkan {
	VkResult begin_one_time_cmd_buffer(VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer, uint32_t qfIndex);

	void end_one_time_cmd_buffer(VkQueue queue, VkCommandPool& cmdPool, VkCommandBuffer& cmdBuffer);

	VkCommandPool create_cmd_pool(const VkCommandPoolCreateFlags& flags, const uint32_t& qf_index);

	VkCommandBuffer create_cmd_buffer(VkCommandPool cmd_pool);

	VkResult begin_cmd_buffer(VkCommandBuffer cmd_buf, VkCommandBufferUsageFlags flags);

	VkFence create_fence(const VkFenceCreateFlags& flags);

	VkSemaphore create_semaphore(const VkSemaphoreType& semaphore_type);

	VkDeviceMemory create_memory(VkDeviceSize size, uint32_t type_index, const void* p_next);

	VkBuffer create_buffer(VkDeviceSize size, VkBufferUsageFlags usage);

	VkPipelineLayout create_pipeline_layout(const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const std::vector<VkPushConstantRange>& push_constant_ranges);

	VkShaderModule create_shader_module(const std::string& file_path);
}