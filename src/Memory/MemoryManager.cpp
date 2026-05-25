#include "MemoryManager.h"
#include "Utility/NameTable.h"
#include "Backend/Swapchain.h"
#include "Utility/Utility.h"
#include "Geometry/TransformMatrices.hpp"
#include "Backend/PhysicalDevice.h"
#include "Texture.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "UniformBuffer.hpp"
#include "StagingBuffer.hpp"

namespace MemoryManager {
	void init() {
		std::vector<uint32_t> gfx_queue_family_index{ PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT) };

		g_resources.add<Texture>(
			NameTable::push_name(texture_name),
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\Sion_Axe_baseColor.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);

		Utility::load_gltf_model(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\scene.gltf)", g_vertices, g_indices);
		const uint64_t VERTEX_BUFFER_SIZE = g_vertices.size() * sizeof(Vertex);
		const uint64_t INDEX_BUFFER_SIZE = g_indices.size() * sizeof(uint32_t);

		g_buffers.add<StagingBuffer>(
			NameTable::push_name(host_vertices_name),
			VERTEX_BUFFER_SIZE,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);
		g_buffers.add<VertexBuffer>(
			NameTable::push_name(device_vertices_name),
			VERTEX_BUFFER_SIZE,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);
		g_buffers.add<StagingBuffer>(
			NameTable::push_name(host_indices_name),
			INDEX_BUFFER_SIZE,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);
		g_buffers.add<IndexBuffer>(
			NameTable::push_name(device_indices_name),
			INDEX_BUFFER_SIZE,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);

		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			g_buffers.add<UniformBuffer>(
				NameTable::push_name(transform_matrices[i]),
				sizeof(TransformMatrices),
				VK_SHARING_MODE_EXCLUSIVE,
				gfx_queue_family_index
			);
		}

		g_depth_image = DepthImage(
			VK_FORMAT_D32_SFLOAT,
			Swapchain::g_status.imageExtent.width,
			Swapchain::g_status.imageExtent.height,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);

		std::vector<Buffer*> p_host_buffers{
			g_buffers.get<StagingBuffer>(NameTable::get_index(host_vertices_name)),
			g_buffers.get<StagingBuffer>(NameTable::get_index(host_indices_name))
		};
		p_host_buffers.reserve(2 + Swapchain::g_IMAGE_COUNT);
		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			p_host_buffers.push_back(g_buffers.get<UniformBuffer>(NameTable::get_index(transform_matrices[i])));
		}
		g_host_memory = HostMemory(p_host_buffers);

		std::vector<Buffer*> p_device_buffers{
			g_buffers.get<VertexBuffer>(NameTable::get_index(device_vertices_name)),
			g_buffers.get<IndexBuffer>(NameTable::get_index(device_indices_name))
		};
		std::vector<Image*> p_device_images{
			g_resources.get<Texture>(NameTable::get_index(texture_name)),
			&g_depth_image
		};
		g_device_memory = DeviceMemory(p_device_buffers, p_device_images);

		g_texture_image_view = ImageView(
			VK_NO_FLAGS,
			g_resources.get<Texture>(NameTable::get_index(texture_name))->get_image(),
			VK_IMAGE_VIEW_TYPE_2D,
			static_cast<VkFormat>(g_resources.get<Texture>(NameTable::get_index(texture_name))->get_p_ktx_texture()->vkFormat),
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);
		g_depth_image_view = ImageView(
			VK_NO_FLAGS,
			g_depth_image.get_image(),
			VK_IMAGE_VIEW_TYPE_2D,
			VK_FORMAT_D32_SFLOAT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 }
		);

		g_host_memory.copy_data_to_buffer(g_buffers.get<StagingBuffer>(NameTable::get_index(host_vertices_name)), g_vertices.data(), VERTEX_BUFFER_SIZE);
		g_host_memory.copy_data_to_buffer(g_buffers.get<StagingBuffer>(NameTable::get_index(host_indices_name)), g_indices.data(), INDEX_BUFFER_SIZE);

		Buffer::copy_buffer(g_buffers.get<StagingBuffer>(NameTable::get_index(host_vertices_name)), g_buffers.get<VertexBuffer>(NameTable::get_index(device_vertices_name)), VkBufferCopy{0, 0, VERTEX_BUFFER_SIZE});
		Buffer::copy_buffer(g_buffers.get<StagingBuffer>(NameTable::get_index(host_indices_name)), g_buffers.get<IndexBuffer>(NameTable::get_index(device_indices_name)), VkBufferCopy{0, 0, INDEX_BUFFER_SIZE});

		Texture::copy_ktx_texture_to_image(g_resources.get<Texture>(NameTable::get_index(texture_name))->get_image(), g_resources.get<Texture>(NameTable::get_index(texture_name))->get_p_ktx_texture());

		VkSamplerCreateInfo sampler_create{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.anisotropyEnable = VK_TRUE,
			.maxAnisotropy = PhysicalDevice::g_limits.maxSamplerAnisotropy,
			.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
			.unnormalizedCoordinates = VK_FALSE
		};
		VK_CHECK(vkCreateSampler(g_device, &sampler_create, nullptr, &g_sampler), "MemoryManager::init(): failed to create sampler")

		g_descriptor_set = DescriptorSet({
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
			}
		});
		g_descriptor_set.write(
			DescriptorSet::Write{
				.binding_num = 0,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				.image_view = g_texture_image_view.get_image_view(),
				.image_layout = VK_IMAGE_LAYOUT_GENERAL
			}
		);
		g_descriptor_set.write(
			DescriptorSet::Write{
				.binding_num = 1,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_SAMPLER,
				.sampler = g_sampler
			}
		);
	}
}