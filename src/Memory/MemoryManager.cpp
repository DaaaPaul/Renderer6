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
#include "Geometry/TransformMatrices.hpp"
#include "Geometry/Vertex.hpp"
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
				Ids::g_TRANSFORM_MATRICES[i],
				sizeof(TransformMatrices),
				VK_SHARING_MODE_EXCLUSIVE,
				gfx_queue_family_index
			);
		}

		g_images.add<Texture>(
			Ids::g_SION_TEXTURE,
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\Sion_Axe_baseColor.ktx2)",
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
		p_host_buffers.reserve(2 + Swapchain::g_IMAGE_COUNT);
		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			p_host_buffers.push_back(g_buffers.get<UniformBuffer>(Ids::g_TRANSFORM_MATRICES[i]));
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
			static_cast<VkFormat>(g_images.get<Texture>(Ids::g_SION_TEXTURE)->get_p_ktx_texture()->vkFormat),
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

		VK_CHECK(Texture::copy_ktx_texture_to_image(g_images.get<Texture>(Ids::g_SION_TEXTURE)->get_image(), g_images.get<Texture>(Ids::g_SION_TEXTURE)->get_p_ktx_texture()), "copy_ktx_texture_to_image: failed");

		g_sampler = Sampler(
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
				.image_view = g_image_views.get<ImageView>(Ids::g_SION_TEXTURE_VIEW)->get_image_view(),
				.image_layout = VK_IMAGE_LAYOUT_GENERAL
			}
		);
		g_descriptor_set.write(
			DescriptorSet::Write{
				.binding_num = 1,
				.descriptor_num = 0,
				.descriptor_count = 1,
				.descriptor_type = VK_DESCRIPTOR_TYPE_SAMPLER,
				.sampler = g_sampler.get_sampler()
			}
		);
	}

	void destroy() {
		g_descriptor_set.destroy();
		g_sampler.destroy();

		g_image_views.remove(Ids::g_DEPTH_VIEW);
		g_image_views.remove(Ids::g_SION_TEXTURE_VIEW);

		g_device_memory_2.destroy();
		g_device_memory_1.destroy();
		g_host_memory.destroy();

		g_images.remove(Ids::g_DEPTH_IMAGE);
		g_images.remove(Ids::g_SION_TEXTURE);

		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			g_buffers.remove(Ids::g_TRANSFORM_MATRICES[i]);
		}
		g_buffers.remove(Ids::g_VERTEX_BUFFER);
		g_buffers.remove(Ids::g_INDEX_BUFFER);
		g_buffers.remove(Ids::g_VERTEX_STAGE);
		g_buffers.remove(Ids::g_INDEX_STAGE);
	}
}