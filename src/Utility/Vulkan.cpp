#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <ktx.h>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <cassert>
#include <cstdint>
#include "Utility/Vulkan.h"
#include "Utility/Utility.h"
#include "Backend/LogicalDevice.h"
#include "Backend/PhysicalDevice.h"
#include "Backend/Instance.h"
#include "Backend/Window.h"

namespace Vulkan {
	void load_gltf_model(const std::string& file_path, std::vector<PBRVertex>& vertices, std::vector<uint32_t>& indices) {
		tinygltf::Model model{};
		tinygltf::TinyGLTF loader{};
		std::string error_msg{};
		std::string warning_msg{};
		bool result{};

		if(file_path.ends_with(".gltf")) {
			result = loader.LoadASCIIFromFile(&model, &error_msg, &warning_msg, file_path);
		} else if(file_path.ends_with(".glb")) {
			result = loader.LoadBinaryFromFile(&model, &error_msg, &warning_msg, file_path);
		} else {
			THROW_RUNTIME("load_gltf_model: invalid file extension")
		}

		if(!error_msg.empty()) {
			PRINTLN("load_gltf_model (error): " << error_msg)
		}
		if(!warning_msg.empty()) {
			PRINTLN("load_gltf_model (warning): " << warning_msg)
		}
		if(!result) {
			THROW_RUNTIME("load_gltf_model: !result")
		}

		vertices.clear();
		indices.clear();

		for(const tinygltf::Mesh& mesh : model.meshes) {
			for(const tinygltf::Primitive& primitive : mesh.primitives) {
				const tinygltf::Accessor& pos_accessor = model.accessors[primitive.attributes.at("POSITION")];
				const tinygltf::BufferView& pos_buffer_view = model.bufferViews[pos_accessor.bufferView];
				const tinygltf::Buffer& pos_buffer = model.buffers[pos_buffer_view.buffer];

				const tinygltf::Accessor& normal_accessor = model.accessors[primitive.attributes.at("NORMAL")];
				const tinygltf::BufferView& normal_buffer_view = model.bufferViews[normal_accessor.bufferView];
				const tinygltf::Buffer& normal_buffer = model.buffers[normal_buffer_view.buffer];

				const tinygltf::Accessor& uv_accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
				const tinygltf::BufferView& uv_buffer_view = model.bufferViews[uv_accessor.bufferView];
				const tinygltf::Buffer& uv_buffer = model.buffers[uv_buffer_view.buffer];

				const tinygltf::Accessor& tangent_accessor = model.accessors[primitive.attributes.at("TANGENT")];
				const tinygltf::BufferView& tangent_buffer_view = model.bufferViews[tangent_accessor.bufferView];
				const tinygltf::Buffer& tangent_buffer = model.buffers[tangent_buffer_view.buffer];

				uint32_t base_vertex_index = static_cast<uint32_t>(vertices.size());
				vertices.reserve(vertices.size() + pos_accessor.count);
				
				const float* p_positions = reinterpret_cast<const float*>(&pos_buffer.data[pos_buffer_view.byteOffset + pos_accessor.byteOffset]);
				const float* p_normals = reinterpret_cast<const float*>(&normal_buffer.data[normal_buffer_view.byteOffset + normal_accessor.byteOffset]);
				const float* p_uv = reinterpret_cast<const float*>(&uv_buffer.data[uv_buffer_view.byteOffset + uv_accessor.byteOffset]);
				const float* p_tangents = reinterpret_cast<const float*>(&tangent_buffer.data[tangent_buffer_view.byteOffset + tangent_accessor.byteOffset]);

				for(int i = 0; i < pos_accessor.count; ++i) {
					vertices.emplace_back(
						glm::vec3(p_positions[i * 3], p_positions[i * 3 + 1], p_positions[i * 3 + 2]),
						glm::vec3(p_normals[i * 3], p_normals[i * 3 + 1], p_normals[i * 3 + 2]),
						glm::vec2(p_uv[i * 2], p_uv[i * 2 + 1]),
						glm::vec4(p_tangents[i * 4], p_tangents[i * 4 + 1], p_tangents[i * 4 + 2], p_tangents[i * 4 + 3])
					);
				}

				const tinygltf::Accessor& index_accessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& index_buffer_view = model.bufferViews[index_accessor.bufferView];
				const tinygltf::Buffer& index_buffer = model.buffers[index_buffer_view.buffer];

				const unsigned char* p_indices = &index_buffer.data[index_buffer_view.byteOffset + index_accessor.byteOffset];
				indices.reserve(indices.size() + index_accessor.count);

				switch(index_accessor.componentType) {
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
						for(int i = 0; i < index_accessor.count; ++i) {
							indices.emplace_back(base_vertex_index + p_indices[i]);
						}
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
						{
							const uint16_t* accurate_p_indices = reinterpret_cast<const uint16_t*>(p_indices);
							for(int i = 0; i < index_accessor.count; ++i) {
								indices.emplace_back(base_vertex_index + accurate_p_indices[i]);
							}
						}
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
						{
							const uint32_t* accurate_p_indices = reinterpret_cast<const uint32_t*>(p_indices);
							for (int i = 0; i < index_accessor.count; ++i) {
								indices.emplace_back(base_vertex_index + accurate_p_indices[i]);
							}
						}
						break;
					default:
						THROW_RUNTIME("load_gltf_model: unsupported index type")
				}
			}
		}
	}

	ktxTexture2* load_ktx_texture(const char* ktx_path) {
		ktxTexture2* ktx_texture{};

		if(ktxTexture2_CreateFromNamedFile(ktx_path, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktx_texture) != KTX_SUCCESS) {
			THROW_RUNTIME("load_ktx_texture: load failure")
		}

		return ktx_texture;
	}

	void transcode_ktx_texture(ktxTexture2* ktx_texture, ktx_transcode_fmt_e target_format) {
		if(ktxTexture2_NeedsTranscoding(ktx_texture)) {
			if(ktxTexture2_TranscodeBasis(ktx_texture, target_format, VK_NO_FLAGS) != KTX_SUCCESS) {
				THROW_RUNTIME("transcode_ktx_texture: compress failure")
			}
		}
	}

	void insert_image_barrier(VkCommandBuffer cmd_buf, VkImage image, VkImageSubresourceRange subresource_range, VkPipelineStageFlags2 stage1, VkAccessFlags2 access1, VkPipelineStageFlags2 stage2, VkAccessFlags2 access2, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t graphics_queue_family_index) {
		VkImageMemoryBarrier2 insert_image_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = stage1,
			.srcAccessMask = access1,
			.dstStageMask = stage2,
			.dstAccessMask = access2,
			.oldLayout = old_layout,
			.newLayout = new_layout,
			.srcQueueFamilyIndex = graphics_queue_family_index,
			.dstQueueFamilyIndex = graphics_queue_family_index,
			.image = image,
			.subresourceRange = subresource_range,
		};

		VkDependencyInfo deps{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &insert_image_barrier,
		};

		vkCmdPipelineBarrier2(cmd_buf, &deps);
	}

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

	VkCommandPool create_cmd_pool(VkCommandPoolCreateFlags flags, uint32_t qf_index) {
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

	VkFence create_fence(VkFenceCreateFlags flags) {
		VkFence fence{};

		VkFenceCreateInfo fence_create{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = flags
		};

		VK_CHECK(vkCreateFence(g_device, &fence_create, nullptr, &fence), "Failed to create fence")

		return fence;
	}

	VkSemaphore create_semaphore(VkSemaphoreType semaphore_type) {
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
		
		uint32_t gfx_queue_family_index = PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT);
		VkBufferCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = size,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &gfx_queue_family_index
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

	VkSurfaceKHR create_surface() {
		VkSurfaceKHR surface{};

		glfwInit();
		VK_CHECK(glfwCreateWindowSurface(Instance::g_instance, Window::g_glfw_window, nullptr, &surface), "create_surface: failed")

		return surface;
	}

	VkDescriptorSetLayout create_descriptor_set_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings, const std::vector<VkDescriptorBindingFlags>& binding_flags) {
		void* p_next = nullptr;
		VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info{};

		if(!binding_flags.empty()) {
			assert(bindings.size() == binding_flags.size());
		
			binding_flags_info = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
				.bindingCount = UINT32(binding_flags.size()),
				.pBindingFlags = binding_flags.data()
			};

			p_next = &binding_flags_info;
		}
		
		VkDescriptorSetLayout layout{};

		VkDescriptorSetLayoutCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = p_next,
			.bindingCount = UINT32(bindings.size()),
			.pBindings = bindings.data()
		};

		VK_CHECK(vkCreateDescriptorSetLayout(g_device, &create, nullptr, &layout), "create_layout: failed")

		return layout;
	}

	VkDescriptorPool create_descriptor_pool(const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& all_bindings) {
		VkDescriptorPool pool{};

		std::vector<VkDescriptorPoolSize> pool_sizes;

		for(const std::vector<VkDescriptorSetLayoutBinding>& set_bindings : all_bindings) {
			for(const VkDescriptorSetLayoutBinding& binding : set_bindings) {
				pool_sizes.emplace_back(binding.descriptorType, binding.descriptorCount);
			}
		}

		VkDescriptorPoolCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets = UINT32(all_bindings.size()),
			.poolSizeCount = UINT32(pool_sizes.size()),
			.pPoolSizes = pool_sizes.data()
		};
		
		VK_CHECK(vkCreateDescriptorPool(g_device, &create, nullptr, &pool), "create_pool: failed")

		return pool;
	}

	VkDescriptorSet create_descriptor_set(VkDescriptorSetLayout layout, VkDescriptorPool pool) {
		VkDescriptorSet set{};

		VkDescriptorSetAllocateInfo create{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = pool,
			.descriptorSetCount = 1,
			.pSetLayouts = &layout,
		};

		VK_CHECK(vkAllocateDescriptorSets(g_device, &create, &set), "create_descriptor_set: failed")

		return set;
	}

	std::vector<VkImage> get_swapchain_images(VkSwapchainKHR swapchain) {
		uint32_t image_count{};
		VK_CHECK(vkGetSwapchainImagesKHR(g_device, swapchain, &image_count, nullptr), "get_swapchain_images: failed")
		std::vector<VkImage> swapchain_images(image_count);
		VK_CHECK(vkGetSwapchainImagesKHR(g_device, swapchain, &image_count, swapchain_images.data()), "get_swapchain_images: failed")

		return swapchain_images;
	}

	std::vector<VkImageView> get_image_views(const std::vector<VkImage>& images, VkImageViewType view_type, VkImageAspectFlags image_aspect, VkFormat format) {
		std::vector<VkImageView> image_views(images.size());
		
		VkImageViewCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = VK_NULL_HANDLE,
			.viewType = view_type,
			.format = format,
			.subresourceRange = VkImageSubresourceRange{image_aspect, 0, 1, 0, 1}
		};
		for(int i = 0; i < images.size(); ++i) {
			create.image = images[i];
			VK_CHECK(vkCreateImageView(g_device, &create, nullptr, &image_views[i]), "get_image_views: failed")
		}

		return image_views;
	}
}