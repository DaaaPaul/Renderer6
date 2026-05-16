#include "Vulkan.h"
#include "Utility.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"

namespace Vulkan {
	VkResult begin_one_time_cmd_buffer(VkCommandPool& cmd_pool, VkCommandBuffer& cmd_buf, uint32_t queue_family_index) {
		cmd_pool = create_cmd_pool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, queue_family_index);
		cmd_buf = create_cmd_buffer(cmd_pool);
		return begin_cmd_buffer(cmd_buf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	}

	void end_one_time_cmd_buffer(VkQueue queue, VkCommandPool& cmd_pool, VkCommandBuffer& cmd_buf) {
		VK_CHECK(vkEndCommandBuffer(cmd_buf), "Failed to end temporary command buffer recording")

		VkFence cmds_done = create_fence(VK_NO_FLAGS);

		VkSubmitInfo commandBufferSubmit{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &cmd_buf,
		};
		VK_CHECK(vkQueueSubmit(queue, 1, &commandBufferSubmit, cmds_done), "Failed to submit temporary command buffer")
		VK_CHECK(vkWaitForFences(g_device, 1, &cmds_done, VK_TRUE, UINT64_MAX), "Failed to wait for copy command done fence")

		vkDestroyFence(g_device, cmds_done, nullptr);
		vkFreeCommandBuffers(g_device, cmd_pool, 1, &cmd_buf);
		vkDestroyCommandPool(g_device, cmd_pool, nullptr);
	}

	VkCommandPool create_cmd_pool(VkCommandPoolCreateFlags const& flags, uint32_t const& qf_index) {
		VkCommandPool cmd_pool{};

		VkCommandPoolCreateInfo cmd_pool_create{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = flags,
			.queueFamilyIndex = qf_index
		};

		VK_CHECK(vkCreateCommandPool(g_device, &cmd_pool_create, nullptr, &cmd_pool), "Failed to create command pool")

		return cmd_pool;
	}

	VkCommandBuffer create_cmd_buffer(VkCommandPool cmd_pool) {
		VkCommandBuffer cmd_buffer{};

		VkCommandBufferAllocateInfo cmd_buffer_create{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = cmd_pool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};

		VK_CHECK(vkAllocateCommandBuffers(g_device, &cmd_buffer_create, &cmd_buffer), "Failed to create command buffer")

		return cmd_buffer;
	}

	VkResult begin_cmd_buffer(VkCommandBuffer cmd_buf, VkCommandBufferUsageFlags flags) {
		if(flags != VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) {
			VK_CHECK(vkResetCommandBuffer(cmd_buf, VK_NO_FLAGS), "begin_cmd_buf: failed to reset command buffer")
		}
		
		VkCommandBufferBeginInfo begin{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .flags = flags };
		return vkBeginCommandBuffer(cmd_buf, &begin);
	}

	VkFence create_fence(VkFenceCreateFlags const& flags) {
		VkFence fence{};

		VkFenceCreateInfo fence_create{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = flags
		};

		VK_CHECK(vkCreateFence(g_device, &fence_create, nullptr, &fence), "Failed to create fence")

		return fence;
	}

	VkSemaphore create_semaphore(VkSemaphoreType const& semaphore_type) {
		VkSemaphore semaphore{};

		VkSemaphoreTypeCreateInfo type_info{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
			.semaphoreType = semaphore_type,
			.initialValue = 0
		};

		VkSemaphoreCreateInfo semaphore_create{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = &type_info
		};

		VK_CHECK(vkCreateSemaphore(g_device, &semaphore_create, nullptr, &semaphore), "Failed to create semaphore")

		return semaphore;
	}

	VkDeviceMemory create_memory(VkDeviceSize size, uint32_t type_index, const void* p_next) {
		VkDeviceMemory memory{};

		VkMemoryAllocateInfo allocate{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = p_next,
			.allocationSize = size,
			.memoryTypeIndex = type_index
		};
		VK_CHECK(vkAllocateMemory(g_device, &allocate, nullptr, &memory), "create_memory: failed")
		
		return memory;
	}

	VkBuffer create_buffer(VkDeviceSize size, VkBufferUsageFlags usage) {
		VkBuffer buffer{};

		VkBufferCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = size,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT)
		};

		VK_CHECK(vkCreateBuffer(g_device, &create, nullptr, &buffer), "create_buffer: failed")

		return buffer;
	}

	VkPipelineLayout create_pipeline_layout(const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const std::vector<VkPushConstantRange>& push_constant_ranges) {
		VkPipelineLayout pipeline_layout{};

		VkPipelineLayoutCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = UINT32(descriptor_set_layouts.size()),
			.pSetLayouts = descriptor_set_layouts.data(),
			.pushConstantRangeCount = UINT32(push_constant_ranges.size()),
			.pPushConstantRanges = push_constant_ranges.data()
		};

		VK_CHECK(vkCreatePipelineLayout(g_device, &create, nullptr, &pipeline_layout), "create_pipeline_layout: failed")

		return pipeline_layout;
	}

	VkShaderModule create_shader_module(const std::string& file_path) {
		VkShaderModule shader_module{};
		
		std::vector<char> file_bytes = Utility::get_file_bytes(file_path);

		VkShaderModuleCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = file_bytes.size(),
			.pCode = reinterpret_cast<uint32_t*>(file_bytes.data())
		};

		VK_CHECK(vkCreateShaderModule(g_device, &create, nullptr, &shader_module), "create_shader_module: failed")

		return shader_module;
	}
}