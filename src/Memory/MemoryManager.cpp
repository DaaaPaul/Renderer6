#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <vector>
#include <chrono>
#include "MemoryManager.h"
#include "utility/Vulkan.h"
#include "utility/Utility.h"
#include "shader/UniformBufferBlock.hpp"
#include "backend/PhysicalDevice.h"
#include "backend/Swapchain.h"
#include "Image.hpp"
#include "Buffer.hpp"

namespace MemoryManager {
	void init() {
		load_vertices_and_indices();
		init_buffers();
		init_textures();
		init_images();
		populate_memory();
		init_image_views();
		init_samplers();
		init_descriptor_sets();
		write_descriptor_sets();
	}

	void destroy() {
		g_descriptor_sets.get("descriptor set")->destroy();
		g_descriptor_sets.get("gui descriptor set")->destroy();

		g_samplers.get("sampler")->destroy();

		g_image_views.get("depth view")->destroy();
		g_image_views.get("sion normals view")->destroy();
		g_image_views.get("sion metallic roughness view")->destroy();
		g_image_views.get("sion texture view")->destroy();

		g_images.get("depth image")->destroy();

		g_ktx_textures.get("sion normals")->destroy();
		g_ktx_textures.get("sion metallic roughness")->destroy();
		g_ktx_textures.get("sion texture")->destroy();

		g_buffers.get("uniform buffer 0")->destroy();
		g_buffers.get("uniform buffer 1")->destroy();
		g_buffers.get("uniform buffer 2")->destroy();
		g_buffers.get("uniform buffer 3")->destroy();
		g_buffers.get("sphere vertex buffer")->destroy();
		g_buffers.get("sphere index buffer")->destroy();
		g_buffers.get("sphere vertex stage")->destroy();
		g_buffers.get("sphere index stage")->destroy();
		g_buffers.get("vertex buffer")->destroy();
		g_buffers.get("index buffer")->destroy();
		g_buffers.get("vertex stage")->destroy();
		g_buffers.get("index stage")->destroy();
	}

	void load_vertices_and_indices() {
		std::pair<std::vector<PBRVertex>, std::vector<uint32_t>> model = Vulkan::load_gltf_model(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\scene.gltf)");
		g_vertices = std::move(model.first);
		g_indices = std::move(model.second);

		std::pair<std::vector<glm::vec3>,  std::vector<uint32_t>> sphere = Utility::get_sphere(0.1f, 4, 4);
		g_sphere_vertices = std::move(sphere.first);
		g_sphere_indices = std::move(sphere.second);
	}

	void init_buffers() {
		const uint64_t VERTEX_BUFFER_SIZE = g_vertices.size() * sizeof(PBRVertex);
		const uint64_t INDEX_BUFFER_SIZE = g_indices.size() * sizeof(uint32_t);

		const uint32_t SIMPLE_VERTEX_BUFFER_SIZE = g_sphere_vertices.size() * sizeof(glm::vec3);
		const uint32_t SIMPLE_INDEX_BUFFER_SIZE = g_sphere_indices.size() * sizeof(uint32_t);
	
		constexpr VmaAllocationCreateInfo DEVICE_HOST_VMA_ALLOCATE{
			.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
			.usage = VMA_MEMORY_USAGE_AUTO
		};

		g_buffers.add(
			"vertex buffer",
			Vulkan::NO_FLAGS,
			VERTEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			DEVICE_HOST_VMA_ALLOCATE
		);
		g_buffers.add(
			"index buffer",
			Vulkan::NO_FLAGS,
			INDEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			DEVICE_HOST_VMA_ALLOCATE
		);
		g_buffers.add(
			"sphere vertex buffer",
			Vulkan::NO_FLAGS,
			SIMPLE_VERTEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			DEVICE_HOST_VMA_ALLOCATE
		);
		g_buffers.add(
			"sphere index buffer",
			Vulkan::NO_FLAGS,
			SIMPLE_INDEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			DEVICE_HOST_VMA_ALLOCATE
		);
		g_buffers.add(
			"uniform buffer 0",
			Vulkan::NO_FLAGS,
			sizeof(UniformBufferBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			DEVICE_HOST_VMA_ALLOCATE
		);
		g_buffers.add(
			"uniform buffer 1",
			Vulkan::NO_FLAGS,
			sizeof(UniformBufferBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			DEVICE_HOST_VMA_ALLOCATE
		);
		g_buffers.add(
			"uniform buffer 2",
			Vulkan::NO_FLAGS,
			sizeof(UniformBufferBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			DEVICE_HOST_VMA_ALLOCATE
		);
		g_buffers.add(
			"uniform buffer 3",
			Vulkan::NO_FLAGS,
			sizeof(UniformBufferBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			DEVICE_HOST_VMA_ALLOCATE
		);
	}

	void init_textures() {
		constexpr VmaAllocationCreateInfo VMA_ALLOCATE{
			.usage = VMA_MEMORY_USAGE_AUTO
		};

		g_ktx_textures.add(
			"sion texture",
			KTX_TTF_BC7_RGBA,
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\base_color.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			VMA_ALLOCATE
		);
		g_ktx_textures.add(
			"sion metallic roughness",
			KTX_TTF_BC7_RGBA,
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\metallic_roughness.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			VMA_ALLOCATE
		);
		g_ktx_textures.add(
			"sion normals",
			KTX_TTF_BC7_RGBA,
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\normal.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			VMA_ALLOCATE
		);
	}

	void init_images() {
		constexpr VmaAllocationCreateInfo VMA_ALLOCATE{
			.usage = VMA_MEMORY_USAGE_AUTO
		};

		g_images.add(
			"depth image",
			Vulkan::NO_FLAGS,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_D32_SFLOAT,
			VkExtent3D{Swapchain::g_status.imageExtent.width, Swapchain::g_status.imageExtent.height, 1},
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index,
			VMA_ALLOCATE
		);
	}

	void populate_memory() {
		const uint64_t VERTEX_BUFFER_SIZE = g_vertices.size() * sizeof(PBRVertex);
		const uint64_t INDEX_BUFFER_SIZE = g_indices.size() * sizeof(uint32_t);

		const uint32_t SPHERE_VERTEX_BUFFER_SIZE = g_sphere_vertices.size() * sizeof(glm::vec3);
		const uint32_t SPHERE_INDEX_BUFFER_SIZE = g_sphere_indices.size() * sizeof(uint32_t);

		Buffer::copy_to(g_vertices.data(), g_buffers.get("vertex buffer"), VERTEX_BUFFER_SIZE);
		Buffer::copy_to(g_indices.data(), g_buffers.get("index buffer"), INDEX_BUFFER_SIZE);
		Buffer::copy_to(g_sphere_vertices.data(), g_buffers.get("sphere vertex buffer"), SPHERE_VERTEX_BUFFER_SIZE);
		Buffer::copy_to(g_sphere_indices.data(), g_buffers.get("sphere index buffer"), SPHERE_INDEX_BUFFER_SIZE);

		Image::transition_layout(g_ktx_textures.get("sion texture")->get_image(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1);
		Image::transition_layout(g_ktx_textures.get("sion metallic roughness")->get_image(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1);
		Image::transition_layout(g_ktx_textures.get("sion normals")->get_image(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1);
		g_ktx_textures.get("sion texture")->copy_texture_data_to_image();
		g_ktx_textures.get("sion metallic roughness")->copy_texture_data_to_image();
		g_ktx_textures.get("sion normals")->copy_texture_data_to_image();
	}

	void init_image_views() {
		g_image_views.add(
			"sion texture view",
			Vulkan::NO_FLAGS,
			g_ktx_textures.get("sion texture")->get_image().get_image(),
			VK_IMAGE_VIEW_TYPE_2D,
			static_cast<VkFormat>(g_ktx_textures.get("sion texture")->get_ktx_texture()->vkFormat),
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);
		g_image_views.add(
			"sion metallic roughness view",
			Vulkan::NO_FLAGS,
			g_ktx_textures.get("sion metallic roughness")->get_image().get_image(),
			VK_IMAGE_VIEW_TYPE_2D,
			static_cast<VkFormat>(g_ktx_textures.get("sion metallic roughness")->get_ktx_texture()->vkFormat),
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);
		g_image_views.add(
			"sion normals view",
			Vulkan::NO_FLAGS,
			g_ktx_textures.get("sion normals")->get_image().get_image(),
			VK_IMAGE_VIEW_TYPE_2D,
			static_cast<VkFormat>(g_ktx_textures.get("sion normals")->get_ktx_texture()->vkFormat),
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);
		g_image_views.add(
			"depth view",
			Vulkan::NO_FLAGS,
			g_images.get("depth image")->get_image(),
			VK_IMAGE_VIEW_TYPE_2D,
			VK_FORMAT_D32_SFLOAT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 }
		);
	}

	void init_samplers() {
		g_samplers.add("sampler", 
			Vulkan::NO_FLAGS,
			VK_FILTER_LINEAR,
			VK_FILTER_LINEAR,
			VK_SAMPLER_MIPMAP_MODE_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			0.0f,
			VK_TRUE,
			PhysicalDevice::g_limits.maxSamplerAnisotropy,
			VK_FALSE,
			VK_COMPARE_OP_NEVER,
			0.0f,
			0.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
		);
	}

	void init_descriptor_sets() {
		g_descriptor_sets.add("descriptor set",
			std::vector<VkDescriptorSetLayoutBinding>{
				VkDescriptorSetLayoutBinding{
					.binding = 0,
					.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
				},
				VkDescriptorSetLayoutBinding{
					.binding = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
				},
				VkDescriptorSetLayoutBinding{
					.binding = 2,
					.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
				},
				VkDescriptorSetLayoutBinding{
					.binding = 3,
					.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
				},
				VkDescriptorSetLayoutBinding{
					.binding = 4,
					.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
				}
			}
		);

		g_descriptor_sets.add("gui descriptor set",
			std::vector<VkDescriptorSetLayoutBinding>{
				VkDescriptorSetLayoutBinding{
					.binding = 0,
					.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
				},
				VkDescriptorSetLayoutBinding{
					.binding = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
				},
			}
		);
	}

	void write_descriptor_sets() {
		DescriptorSet* p_descriptor_set = g_descriptor_sets.get("descriptor set");
		p_descriptor_set->write(
			DescriptorSet::Write{
				.binding_num = 0,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.buffer_info = VkDescriptorBufferInfo{
					.buffer = g_buffers.get("uniform buffer 0")->get_buffer(),
					.offset = 0,
					.range = VK_WHOLE_SIZE
				}
			}
		);
		p_descriptor_set->write(
			DescriptorSet::Write{
				.binding_num = 1,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_SAMPLER,
				.image_info = VkDescriptorImageInfo{
					.sampler = g_samplers.get("sampler")->get_sampler()
				}
			}
		);
		p_descriptor_set->write(
			DescriptorSet::Write{
				.binding_num = 2,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				.image_info = VkDescriptorImageInfo{
					.imageView = g_image_views.get("sion texture view")->get_image_view(),
					.imageLayout = VK_IMAGE_LAYOUT_GENERAL
				}
			}
		);
		p_descriptor_set->write(
			DescriptorSet::Write{
				.binding_num = 3,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				.image_info = VkDescriptorImageInfo{
					.imageView = g_image_views.get("sion metallic roughness view")->get_image_view(),
					.imageLayout = VK_IMAGE_LAYOUT_GENERAL
				}
			}
		);
		p_descriptor_set->write(
			DescriptorSet::Write{
				.binding_num = 4,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				.image_info = VkDescriptorImageInfo{
					.imageView = g_image_views.get("sion normals view")->get_image_view(),
					.imageLayout = VK_IMAGE_LAYOUT_GENERAL
				}
			}
		);

		DescriptorSet* p_gui_descriptor_set = g_descriptor_sets.get("gui descriptor set");
		p_gui_descriptor_set->write(
			DescriptorSet::Write{
				.binding_num = 1,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_SAMPLER,
				.image_info = VkDescriptorImageInfo{
					.sampler = g_samplers.get("sampler")->get_sampler()
				}
			}
		);
	}
}