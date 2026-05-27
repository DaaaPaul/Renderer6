#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include <GLFW/glfw3.h>
#include "Utility/Vulkan.h"
#include "Utility/Utility.h"
#include "Backend/LogicalDevice.h"
#include "Backend/PhysicalDevice.h"
#include "Backend/Instance.h"
#include "Geometry/Vertex.hpp"
#include "Backend/Window.h"

namespace Vulkan {
	void load_gltf_model(const char* file_path, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
		tinygltf::Model model{};
		tinygltf::TinyGLTF loader{};
		std::string error{};
		std::string warning{};

		if(!loader.LoadASCIIFromFile(&model, &error, &warning, file_path)) {
			throw std::runtime_error("load_gltf_model failed: " + error);
		}
		if(!warning.empty()) {
			PRINTLN("load_gltf_model warning: " << warning);
		}

		// process all meshes in the model
		std::unordered_map<Vertex, uint32_t> unique_vertices{};

		for (const tinygltf::Mesh& mesh : model.meshes) {
			for (const tinygltf::Primitive& primitive : mesh.primitives) {
				// get indices
				const tinygltf::Accessor& index_accessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& index_buffer_view = model.bufferViews[index_accessor.bufferView];
				const tinygltf::Buffer& index_buffer = model.buffers[index_buffer_view.buffer];

				// get vertex positions
				const tinygltf::Accessor& position_accessor = model.accessors[primitive.attributes.at("POSITION")];
				const tinygltf::BufferView& position_buffer_view = model.bufferViews[position_accessor.bufferView];
				const tinygltf::Buffer& position_buffer = model.buffers[position_buffer_view.buffer];

				// get texture coordinates if available
				bool has_tex_coords = primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end();
				const tinygltf::Accessor* p_texcoord_accessor = nullptr;
				const tinygltf::BufferView* p_texcoord_buffer_view = nullptr;
				const tinygltf::Buffer* p_texcoord_buffer = nullptr;

				if (has_tex_coords) {
					p_texcoord_accessor = &model.accessors[primitive.attributes.at("TEXCOORD_0")];
					p_texcoord_buffer_view = &model.bufferViews[p_texcoord_accessor->bufferView];
					p_texcoord_buffer = &model.buffers[p_texcoord_buffer_view->buffer];
				}

				// process vertices
				for (int i = 0; i < position_accessor.count; ++i) {
					Vertex vertex{};

					// get position
					const float* p_position = reinterpret_cast<const float*>(&position_buffer.data[position_buffer_view.byteOffset + position_accessor.byteOffset + i * 12]);
					vertex.position = {p_position[0], p_position[1], p_position[2], 1.0f};

					// get texture coordinates if available
					if (has_tex_coords) {
						const float* p_texcoord = reinterpret_cast<const float*>(&p_texcoord_buffer->data[p_texcoord_buffer_view->byteOffset + p_texcoord_accessor->byteOffset + i * 8]);
						vertex.tex_coord = {p_texcoord[0], p_texcoord[1]};
					} else {
						vertex.tex_coord = {0.0f, 0.0f};
					}

					// add vertex if unique
					if (!unique_vertices.contains(vertex)) {
						unique_vertices[vertex] = UINT32(vertices.size());
						vertices.push_back(vertex);
					}
				}

				// process indices
				const unsigned char* p_index_data = &index_buffer.data[index_buffer_view.byteOffset + index_accessor.byteOffset];

				// handle different index component types
				if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
					const uint16_t* p_indices = reinterpret_cast<const uint16_t*>(p_index_data);
					for (size_t i = 0; i < index_accessor.count; ++i) {
						indices.push_back(unique_vertices[vertices[p_indices[i]]]);
					}
				} else if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
					const uint32_t* p_indices = reinterpret_cast<const uint32_t*>(p_index_data);
					for (size_t i = 0; i < index_accessor.count; ++i) {
						indices.push_back(unique_vertices[vertices[p_indices[i]]]);
					}
				} else if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
					const uint8_t* p_indices = reinterpret_cast<const uint8_t*>(p_index_data);
					for (size_t i = 0; i < index_accessor.count; ++i) {
						indices.push_back(unique_vertices[vertices[p_indices[i]]]);
					}
				}
			}
		}
	}

	ktxTexture2* load_ktx_texture(const char* ktx_path) {
		ktxTexture2* p_ktx_texture{};
		KTX_error_code error = ktxTexture2_CreateFromNamedFile(ktx_path, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &p_ktx_texture);

		if(error != KTX_SUCCESS) {
			throw std::runtime_error("load_ktx_texture: load failure");
		} else {
			if(ktxTexture2_NeedsTranscoding(p_ktx_texture)) {
				constexpr ktx_transcode_fmt_e TARGET_FORMAT = KTX_TTF_BC7_RGBA;

				if(ktxTexture2_TranscodeBasis(p_ktx_texture, TARGET_FORMAT, 0) != KTX_SUCCESS) {
					throw std::runtime_error("load_ktx_texture: compress failure");
				}
			}
		}

		return p_ktx_texture;
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

	VkDescriptorPool create_one_set_pool(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
		VkDescriptorPool pool{};
		std::vector<VkDescriptorPoolSize> poolSizes(bindings.size());

		for(int i = 0; i < bindings.size(); ++i) {
			poolSizes[i] = VkDescriptorPoolSize{
				.type = bindings[i].descriptorType, 
				.descriptorCount = bindings[i].descriptorCount
			};
		}
		VkDescriptorPoolCreateInfo create{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = 1,
			.poolSizeCount = UINT32(poolSizes.size()),
			.pPoolSizes = poolSizes.data()
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