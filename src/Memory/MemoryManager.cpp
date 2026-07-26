#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <vector>
#include <chrono>
#include "MemoryManager.h"
#include "Utility/Ids.h"
#include "Utility/Vulkan.h"
#include "Utility/Utility.h"
#include "ShaderStructs/UniformBufferBlock.hpp"
#include "Backend/PhysicalDevice.h"
#include "Image.hpp"
#include "KtxTexture.hpp"
#include "Buffer.hpp"

namespace MemoryManager {
	void load_vertices_and_indices() {
		std::pair<std::vector<PBRVertex>, std::vector<uint32_t>> model = Vulkan::load_gltf_model(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\scene.gltf)");
		g_vertices = std::move(model.first);
		g_indices = std::move(model.second);

		std::pair<std::vector<glm::vec3>,  std::vector<uint32_t>> sphere = Utility::get_sphere(0.1f, 4, 4);
		g_simple_vertices = std::move(sphere.first);
		g_simple_indices = std::move(sphere.second);
	}

	void init_buffers() {
		const uint64_t VERTEX_BUFFER_SIZE = g_vertices.size() * sizeof(PBRVertex);
		const uint64_t INDEX_BUFFER_SIZE = g_indices.size() * sizeof(uint32_t);

		const uint32_t SIMPLE_VERTEX_BUFFER_SIZE = g_simple_vertices.size() * sizeof(glm::vec3);
		const uint32_t SIMPLE_INDEX_BUFFER_SIZE = g_simple_indices.size() * sizeof(uint32_t);
	
		g_buffers.add<Buffer>(
			Ids::g_VERTEX_STAGE,
			Vulkan::NO_FLAGS,
			VERTEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
		g_buffers.add<Buffer>(
			Ids::g_INDEX_STAGE,
			Vulkan::NO_FLAGS,
			INDEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
		g_buffers.add<Buffer>(
			Ids::g_VERTEX_BUFFER,
			Vulkan::NO_FLAGS,
			VERTEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
		g_buffers.add<Buffer>(
			Ids::g_INDEX_BUFFER,
			Vulkan::NO_FLAGS,
			INDEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);

		g_buffers.add<Buffer>(
			Ids::g_SIMPLE_VERTEX_STAGE,
			Vulkan::NO_FLAGS,
			SIMPLE_VERTEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
		g_buffers.add<Buffer>(
			Ids::g_SIMPLE_INDEX_STAGE,
			Vulkan::NO_FLAGS,
			SIMPLE_INDEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
		g_buffers.add<Buffer>(
			Ids::g_SIMPLE_VERTEX_BUFFER,
			Vulkan::NO_FLAGS,
			SIMPLE_VERTEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
		g_buffers.add<Buffer>(
			Ids::g_SIMPLE_INDEX_BUFFER,
			Vulkan::NO_FLAGS,
			SIMPLE_INDEX_BUFFER_SIZE,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);

		g_buffers.add<Buffer>(
			Ids::g_UNIFORM_BUFFERS[0],
			Vulkan::NO_FLAGS,
			sizeof(UniformBufferBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
		g_buffers.add<Buffer>(
			Ids::g_UNIFORM_BUFFERS[1],
			Vulkan::NO_FLAGS,
			sizeof(UniformBufferBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
		g_buffers.add<Buffer>(
			Ids::g_UNIFORM_BUFFERS[2],
			Vulkan::NO_FLAGS,
			sizeof(UniformBufferBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
		g_buffers.add<Buffer>(
			Ids::g_UNIFORM_BUFFERS[3],
			Vulkan::NO_FLAGS,
			sizeof(UniformBufferBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
	}

	void init_images() {
		g_images.add<Image>(
			Ids::g_DEPTH_IMAGE,
			Vulkan::NO_FLAGS,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_D32_SFLOAT,
			VkExtent3D{Swapchain::g_status.imageExtent.width, Swapchain::g_status.imageExtent.height, 1},
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
	}

	void init_textures() {
		g_textures.add<KtxTexture>(
			Ids::g_SION_TEXTURE,
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\base_color.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index
		);
		g_textures.add<KtxTexture>(
			Ids::g_SION_METALLIC_ROUGHNESS,
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\metallic_roughness.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index		
		);
		g_textures.add<KtxTexture>(
			Ids::g_SION_NORMALS,
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\normal.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			PhysicalDevice::g_graphics_family_index		
		);
	}

	void init_memory() {
		g_host_memory = Memory({
			*g_buffers.get<Buffer>(Ids::g_VERTEX_STAGE),
			*g_buffers.get<Buffer>(Ids::g_INDEX_STAGE),
			*g_buffers.get<Buffer>(Ids::g_SIMPLE_VERTEX_STAGE),
			*g_buffers.get<Buffer>(Ids::g_SIMPLE_INDEX_STAGE),
			*g_buffers.get<Buffer>(Ids::g_UNIFORM_BUFFERS[0]),
			*g_buffers.get<Buffer>(Ids::g_UNIFORM_BUFFERS[1]),
			*g_buffers.get<Buffer>(Ids::g_UNIFORM_BUFFERS[2]),
			*g_buffers.get<Buffer>(Ids::g_UNIFORM_BUFFERS[3])
		}, 
		{}, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, nullptr);

		g_device_memory = Memory({
			*g_buffers.get<Buffer>(Ids::g_VERTEX_BUFFER),
			*g_buffers.get<Buffer>(Ids::g_INDEX_BUFFER),
			*g_buffers.get<Buffer>(Ids::g_SIMPLE_VERTEX_BUFFER),
			*g_buffers.get<Buffer>(Ids::g_SIMPLE_INDEX_BUFFER)
		}, 
		{
			g_textures.get<KtxTexture>(Ids::g_SION_TEXTURE)->image,
			g_textures.get<KtxTexture>(Ids::g_SION_METALLIC_ROUGHNESS)->image,
			g_textures.get<KtxTexture>(Ids::g_SION_NORMALS)->image,
		}, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);

		g_depth_image_memory = Memory(
		{},
		{ *g_images.get<Image>(Ids::g_DEPTH_IMAGE) },
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
	}

	void populate_memory() {
		const uint64_t VERTEX_BUFFER_SIZE = g_vertices.size() * sizeof(PBRVertex);
		const uint64_t INDEX_BUFFER_SIZE = g_indices.size() * sizeof(uint32_t);

		const uint32_t SIMPLE_VERTEX_BUFFER_SIZE = g_simple_vertices.size() * sizeof(glm::vec3);
		const uint32_t SIMPLE_INDEX_BUFFER_SIZE = g_simple_indices.size() * sizeof(uint32_t);

		g_host_memory.copy_to_buffer(g_vertices.data(), VERTEX_BUFFER_SIZE, *g_buffers.get<Buffer>(Ids::g_VERTEX_STAGE));
		g_host_memory.copy_to_buffer(g_indices.data(), INDEX_BUFFER_SIZE, *g_buffers.get<Buffer>(Ids::g_INDEX_STAGE));
		g_host_memory.copy_to_buffer(g_simple_vertices.data(), SIMPLE_VERTEX_BUFFER_SIZE, *g_buffers.get<Buffer>(Ids::g_SIMPLE_VERTEX_STAGE));
		g_host_memory.copy_to_buffer(g_simple_indices.data(), SIMPLE_INDEX_BUFFER_SIZE, *g_buffers.get<Buffer>(Ids::g_SIMPLE_INDEX_STAGE));

		Buffer::copy_buffer(g_buffers.get<Buffer>(Ids::g_VERTEX_STAGE), g_buffers.get<Buffer>(Ids::g_VERTEX_BUFFER), VkBufferCopy{0, 0, VERTEX_BUFFER_SIZE});
		Buffer::copy_buffer(g_buffers.get<Buffer>(Ids::g_INDEX_STAGE), g_buffers.get<Buffer>(Ids::g_INDEX_BUFFER), VkBufferCopy{0, 0, INDEX_BUFFER_SIZE});
		Buffer::copy_buffer(g_buffers.get<Buffer>(Ids::g_SIMPLE_VERTEX_STAGE), g_buffers.get<Buffer>(Ids::g_SIMPLE_VERTEX_BUFFER), VkBufferCopy{0, 0, SIMPLE_VERTEX_BUFFER_SIZE});
		Buffer::copy_buffer(g_buffers.get<Buffer>(Ids::g_SIMPLE_INDEX_STAGE), g_buffers.get<Buffer>(Ids::g_SIMPLE_INDEX_BUFFER), VkBufferCopy{0, 0, SIMPLE_INDEX_BUFFER_SIZE});
		
		Image::transition_image_layout(g_textures.get<KtxTexture>(Ids::g_SION_TEXTURE)->image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1);
		Image::transition_image_layout(g_textures.get<KtxTexture>(Ids::g_SION_METALLIC_ROUGHNESS)->image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1);
		Image::transition_image_layout(g_textures.get<KtxTexture>(Ids::g_SION_NORMALS)->image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, 1);
		g_textures.get<KtxTexture>(Ids::g_SION_TEXTURE)->copy_texture_data_to_image();
		g_textures.get<KtxTexture>(Ids::g_SION_METALLIC_ROUGHNESS)->copy_texture_data_to_image();
		g_textures.get<KtxTexture>(Ids::g_SION_NORMALS)->copy_texture_data_to_image();
	}

	void init_image_views() {
		g_image_views.add<ImageView>(
			Ids::g_SION_TEXTURE_VIEW,
			Vulkan::NO_FLAGS,
			g_textures.get<KtxTexture>(Ids::g_SION_TEXTURE)->image.image,
			VK_IMAGE_VIEW_TYPE_2D,
			static_cast<VkFormat>(g_textures.get<KtxTexture>(Ids::g_SION_TEXTURE)->ktx_texture->vkFormat),
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);
		g_image_views.add<ImageView>(
			Ids::g_SION_METALLIC_ROUGHNESS_VIEW,
			Vulkan::NO_FLAGS,
			g_textures.get<KtxTexture>(Ids::g_SION_METALLIC_ROUGHNESS)->image.image,
			VK_IMAGE_VIEW_TYPE_2D,
			static_cast<VkFormat>(g_textures.get<KtxTexture>(Ids::g_SION_METALLIC_ROUGHNESS)->ktx_texture->vkFormat),
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);
		g_image_views.add<ImageView>(
			Ids::g_SION_NORMALS_VIEW,
			Vulkan::NO_FLAGS,
			g_textures.get<KtxTexture>(Ids::g_SION_NORMALS)->image.image,
			VK_IMAGE_VIEW_TYPE_2D,
			static_cast<VkFormat>(g_textures.get<KtxTexture>(Ids::g_SION_NORMALS)->ktx_texture->vkFormat),
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);
		g_image_views.add<ImageView>(
			Ids::g_DEPTH_VIEW,
			Vulkan::NO_FLAGS,
			g_images.get<Image>(Ids::g_DEPTH_IMAGE)->image,
			VK_IMAGE_VIEW_TYPE_2D,
			VK_FORMAT_D32_SFLOAT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 }
		);
	}

	void init_samplers() {
		g_samplers.add<Sampler>(Ids::g_SAMPLER, 
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
		g_descriptor_sets.add<DescriptorSet>(Ids::g_DESCRIPTOR_SET,
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

		g_descriptor_sets.add<DescriptorSet>(Ids::g_GUI_DESCRIPTOR_SET,
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
		DescriptorSet* p_descriptor_set = g_descriptor_sets.get<DescriptorSet>(Ids::g_DESCRIPTOR_SET);
		p_descriptor_set->write(
			DescriptorSet::Write{
				.binding_num = 0,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.buffer_info = VkDescriptorBufferInfo{
					.buffer = g_buffers.get<Buffer>(Ids::g_UNIFORM_BUFFERS[0])->buffer,
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
					.sampler = g_samplers.get<Sampler>(Ids::g_SAMPLER)->get_sampler()
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
					.imageView = g_image_views.get<ImageView>(Ids::g_SION_TEXTURE_VIEW)->get_image_view(),
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
					.imageView = g_image_views.get<ImageView>(Ids::g_SION_METALLIC_ROUGHNESS_VIEW)->get_image_view(),
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
					.imageView = g_image_views.get<ImageView>(Ids::g_SION_NORMALS_VIEW)->get_image_view(),
					.imageLayout = VK_IMAGE_LAYOUT_GENERAL
				}
			}
		);

		DescriptorSet* p_gui_descriptor_set = g_descriptor_sets.get<DescriptorSet>(Ids::g_GUI_DESCRIPTOR_SET);
		p_gui_descriptor_set->write(
			DescriptorSet::Write{
				.binding_num = 1,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_SAMPLER,
				.image_info = VkDescriptorImageInfo{
					.sampler = g_samplers.get<Sampler>(Ids::g_SAMPLER)->get_sampler()
				}
			}
		);
	}

	void init() {
		load_vertices_and_indices();
		init_buffers();
		init_images();
		init_textures();
		init_memory();
		populate_memory();
		init_image_views();
		init_samplers();
		init_descriptor_sets();
		write_descriptor_sets();
	}

	void destroy() {
		g_descriptor_sets.get<DescriptorSet>(Ids::g_DESCRIPTOR_SET)->destroy();
		g_descriptor_sets.get<DescriptorSet>(Ids::g_GUI_DESCRIPTOR_SET)->destroy();

		g_samplers.get<Sampler>(Ids::g_SAMPLER)->destroy();

		g_image_views.get<ImageView>(Ids::g_DEPTH_VIEW)->destroy();
		g_image_views.get<ImageView>(Ids::g_SION_NORMALS_VIEW)->destroy();
		g_image_views.get<ImageView>(Ids::g_SION_METALLIC_ROUGHNESS_VIEW)->destroy();
		g_image_views.get<ImageView>(Ids::g_SION_TEXTURE_VIEW)->destroy();

		g_depth_image_memory.destroy();
		g_device_memory.destroy();
		g_host_memory.destroy();

		g_images.get<Image>(Ids::g_DEPTH_IMAGE)->destroy();

		g_textures.get<KtxTexture>(Ids::g_SION_NORMALS)->destroy();
		g_textures.get<KtxTexture>(Ids::g_SION_METALLIC_ROUGHNESS)->destroy();
		g_textures.get<KtxTexture>(Ids::g_SION_TEXTURE)->destroy();

		g_buffers.get<Buffer>(Ids::g_UNIFORM_BUFFERS[0])->destroy();
		g_buffers.get<Buffer>(Ids::g_UNIFORM_BUFFERS[1])->destroy();
		g_buffers.get<Buffer>(Ids::g_UNIFORM_BUFFERS[2])->destroy();
		g_buffers.get<Buffer>(Ids::g_UNIFORM_BUFFERS[3])->destroy();
		g_buffers.get<Buffer>(Ids::g_SIMPLE_VERTEX_BUFFER)->destroy();
		g_buffers.get<Buffer>(Ids::g_SIMPLE_INDEX_BUFFER)->destroy();
		g_buffers.get<Buffer>(Ids::g_SIMPLE_VERTEX_STAGE)->destroy();
		g_buffers.get<Buffer>(Ids::g_SIMPLE_INDEX_STAGE)->destroy();
		g_buffers.get<Buffer>(Ids::g_VERTEX_BUFFER)->destroy();
		g_buffers.get<Buffer>(Ids::g_INDEX_BUFFER)->destroy();
		g_buffers.get<Buffer>(Ids::g_VERTEX_STAGE)->destroy();
		g_buffers.get<Buffer>(Ids::g_INDEX_STAGE)->destroy();
	}
}