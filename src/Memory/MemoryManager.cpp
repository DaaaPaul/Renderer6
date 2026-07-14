#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <vector>
#include <chrono>
#include "MemoryManager.h"
#include "HostMemory.hpp"
#include "DeviceMemory.hpp"
#include "Utility/Ids.h"
#include "Utility/Vulkan.h"
#include "Utility/Utility.h"
#include "ShaderStructs/TransformMatrices.hpp"
#include "ShaderStructs/UniformBufferBlock.hpp"
#include "ShaderStructs/Vertex.hpp"
#include "Backend/PhysicalDevice.h"
#include "Backend/Swapchain.h"
#include "Image.hpp"
#include "Texture.hpp"
#include "DepthImage.hpp"
#include "ImageView.hpp"
#include "Buffer.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "UniformBuffer.hpp"
#include "StagingBuffer.hpp"
#include "DescriptorSet.hpp"
#include "Sampler.hpp"

namespace MemoryManager {
	void init() {
		std::vector<uint32_t> gfx_queue_family_index{ PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT) };

		Vulkan::load_gltf_model(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\sion_axe.glb)", g_vertices, g_indices);

		const uint64_t VERTEX_BUFFER_SIZE = g_vertices.size() * sizeof(Vertex);
		const uint64_t INDEX_BUFFER_SIZE = g_indices.size() * sizeof(uint32_t);

		g_buffers.add<StagingBuffer>(
			Ids::g_VERTEX_STAGE,
			VERTEX_BUFFER_SIZE,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);
		g_buffers.add<StagingBuffer>(
			Ids::g_INDEX_STAGE,
			INDEX_BUFFER_SIZE,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);
		g_buffers.add<VertexBuffer>(
			Ids::g_VERTEX_BUFFER,
			VERTEX_BUFFER_SIZE,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);
		g_buffers.add<IndexBuffer>(
			Ids::g_INDEX_BUFFER,
			INDEX_BUFFER_SIZE,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);

		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			g_buffers.add<UniformBuffer>(
				Ids::g_UNIFORM_BUFFERS[i],
				sizeof(UniformBufferBlock),
				VK_SHARING_MODE_EXCLUSIVE,
				gfx_queue_family_index
			);
		}

		g_images.add<Texture>(
			Ids::g_SION_TEXTURE,
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\base_color.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);
		g_images.add<Texture>(
			Ids::g_SION_METALLIC_ROUGHNESS,
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\metallic_roughness.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index		
		);
		g_images.add<Texture>(
			Ids::g_SION_NORMALS,
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\normal.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index		
		);

		g_images.add<DepthImage>(
			Ids::g_DEPTH_IMAGE,
			VK_FORMAT_D32_SFLOAT,
			Swapchain::g_status.imageExtent.width,
			Swapchain::g_status.imageExtent.height,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);

		std::vector<Buffer*> p_host_buffers{
			g_buffers.get<StagingBuffer>(Ids::g_VERTEX_STAGE),
			g_buffers.get<StagingBuffer>(Ids::g_INDEX_STAGE)
		};
		p_host_buffers.reserve(p_host_buffers.size() + Swapchain::g_IMAGE_COUNT);
		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			p_host_buffers.push_back(g_buffers.get<UniformBuffer>(Ids::g_UNIFORM_BUFFERS[i]));
		}
		std::vector<Image*> p_host_images{

		};
		g_host_memory = HostMemory(p_host_buffers, p_host_images);

		std::vector<Buffer*> p_device_buffers{
			g_buffers.get<VertexBuffer>(Ids::g_VERTEX_BUFFER),
			g_buffers.get<IndexBuffer>(Ids::g_INDEX_BUFFER)
		};
		std::vector<Image*> p_device_images{
			g_images.get<Texture>(Ids::g_SION_TEXTURE),
			g_images.get<Texture>(Ids::g_SION_METALLIC_ROUGHNESS),
			g_images.get<Texture>(Ids::g_SION_NORMALS)
		};
		g_device_memory_1 = DeviceMemory(p_device_buffers, p_device_images);

		std::vector<Buffer*> p_device_2_buffers{

		};
		std::vector<Image*> p_device_2_images{
			g_images.get<DepthImage>(Ids::g_DEPTH_IMAGE)
		};
		g_device_memory_2 = DeviceMemory(p_device_2_buffers, p_device_2_images);

		g_image_views.add<ImageView>(
			Ids::g_SION_TEXTURE_VIEW,
			VK_NO_FLAGS,
			g_images.get<Texture>(Ids::g_SION_TEXTURE)->get_image(),
			VK_IMAGE_VIEW_TYPE_2D,
			static_cast<VkFormat>(g_images.get<Texture>(Ids::g_SION_TEXTURE)->get_ktx_texture()->vkFormat),
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);
		g_image_views.add<ImageView>(
			Ids::g_SION_METALLIC_ROUGHNESS_VIEW,
			VK_NO_FLAGS,
			g_images.get<Texture>(Ids::g_SION_METALLIC_ROUGHNESS)->get_image(),
			VK_IMAGE_VIEW_TYPE_2D,
			static_cast<VkFormat>(g_images.get<Texture>(Ids::g_SION_METALLIC_ROUGHNESS)->get_ktx_texture()->vkFormat),
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);
		g_image_views.add<ImageView>(
			Ids::g_SION_NORMALS_VIEW,
			VK_NO_FLAGS,
			g_images.get<Texture>(Ids::g_SION_NORMALS)->get_image(),
			VK_IMAGE_VIEW_TYPE_2D,
			static_cast<VkFormat>(g_images.get<Texture>(Ids::g_SION_NORMALS)->get_ktx_texture()->vkFormat),
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);
		g_image_views.add<ImageView>(
			Ids::g_DEPTH_VIEW,
			VK_NO_FLAGS,
			g_images.get<DepthImage>(Ids::g_DEPTH_IMAGE)->get_image(),
			VK_IMAGE_VIEW_TYPE_2D,
			VK_FORMAT_D32_SFLOAT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 }
		);

		g_host_memory.copy_data_to_buffer(g_buffers.get<StagingBuffer>(Ids::g_VERTEX_STAGE), g_vertices.data(), VERTEX_BUFFER_SIZE);
		g_host_memory.copy_data_to_buffer(g_buffers.get<StagingBuffer>(Ids::g_INDEX_STAGE), g_indices.data(), INDEX_BUFFER_SIZE);

		Buffer::copy_buffer(g_buffers.get<StagingBuffer>(Ids::g_VERTEX_STAGE), g_buffers.get<VertexBuffer>(Ids::g_VERTEX_BUFFER), VkBufferCopy{0, 0, VERTEX_BUFFER_SIZE});
		Buffer::copy_buffer(g_buffers.get<StagingBuffer>(Ids::g_INDEX_STAGE), g_buffers.get<IndexBuffer>(Ids::g_INDEX_BUFFER), VkBufferCopy{0, 0, INDEX_BUFFER_SIZE});

		Texture::copy_ktx_texture_to_image(g_images.get<Texture>(Ids::g_SION_TEXTURE)->get_image(), g_images.get<Texture>(Ids::g_SION_TEXTURE)->get_ktx_texture());
		Texture::copy_ktx_texture_to_image(g_images.get<Texture>(Ids::g_SION_METALLIC_ROUGHNESS)->get_image(), g_images.get<Texture>(Ids::g_SION_METALLIC_ROUGHNESS)->get_ktx_texture());
		Texture::copy_ktx_texture_to_image(g_images.get<Texture>(Ids::g_SION_NORMALS)->get_image(), g_images.get<Texture>(Ids::g_SION_NORMALS)->get_ktx_texture());

		g_samplers.add<Sampler>(Ids::g_SAMPLER, 
			VK_NO_FLAGS,
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
		Sampler* p_sampler = g_samplers.get<Sampler>(Ids::g_SAMPLER);

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

		DescriptorSet* p_descriptor_set = g_descriptor_sets.get<DescriptorSet>(Ids::g_DESCRIPTOR_SET);
		p_descriptor_set->write(
			DescriptorSet::Write{
				.binding_num = 0,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.buffer_info = VkDescriptorBufferInfo{
					.buffer = g_buffers.get<UniformBuffer>(Ids::g_UNIFORM_BUFFERS[0])->get_buffer(),
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
					.sampler = p_sampler->get_sampler()
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
	}

	void destroy() {
		g_descriptor_sets.get<DescriptorSet>(Ids::g_DESCRIPTOR_SET)->destroy();
		g_samplers.get<Sampler>(Ids::g_SAMPLER)->destroy();
		g_descriptor_sets.remove(Ids::g_DESCRIPTOR_SET);
		g_descriptor_sets.remove(Ids::g_SAMPLER);

		g_image_views.remove(Ids::g_DEPTH_VIEW);
		g_image_views.remove(Ids::g_SION_NORMALS_VIEW);
		g_image_views.remove(Ids::g_SION_METALLIC_ROUGHNESS_VIEW);
		g_image_views.remove(Ids::g_SION_TEXTURE_VIEW);

		g_device_memory_2.destroy();
		g_device_memory_1.destroy();
		g_host_memory.destroy();

		g_images.remove(Ids::g_DEPTH_IMAGE);
		g_images.remove(Ids::g_SION_NORMALS);
		g_images.remove(Ids::g_SION_METALLIC_ROUGHNESS);
		g_images.remove(Ids::g_SION_TEXTURE);

		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			g_buffers.remove(Ids::g_UNIFORM_BUFFERS[i]);
		}
		g_buffers.remove(Ids::g_VERTEX_BUFFER);
		g_buffers.remove(Ids::g_INDEX_BUFFER);
		g_buffers.remove(Ids::g_VERTEX_STAGE);
		g_buffers.remove(Ids::g_INDEX_STAGE);
	}
}