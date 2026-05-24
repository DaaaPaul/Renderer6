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

namespace MemoryManager {
	void init() {
		std::vector<uint32_t> gfx_queue_family_index{ PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT) };

		g_resources.add<Texture>(
			NameTable::push_name("sion axe texture"),
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\Sion_Axe_baseColor.ktx2)",
			1,
			1,
			VK_SAMPLE_COUNT_1_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);

		Utility::load_gltf_model(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\scene.gltf)", g_vertices, g_indices);
		const uint64_t vertex_buffer_size = g_vertices.size() * sizeof(Vertex);
		const uint64_t index_buffer_size = g_indices.size() * sizeof(uint32_t);

		g_buffers.add<VertexBuffer>(
			NameTable::push_name("host sion axe vertices"),
			vertex_buffer_size,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);
		g_buffers.add<VertexBuffer>(
			NameTable::push_name("device sion axe vertices"),
			vertex_buffer_size,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);
		g_buffers.add<IndexBuffer>(
			NameTable::push_name("host sion axe indices"),
			index_buffer_size,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);
		g_buffers.add<IndexBuffer>(
			NameTable::push_name("device sion axe indices"),
			index_buffer_size,
			VK_SHARING_MODE_EXCLUSIVE,
			gfx_queue_family_index
		);

		std::string name = "transform matrices ";
		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; ++i) {
			std::string num = std::to_string(i + 1);
			g_buffers.add<UniformBuffer>(
				NameTable::push_name((name + num).c_str()),
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

		std::vector<Buffer> host_buffers;
		host_buffers.reserve(2 + Swapchain::g_IMAGE_COUNT);
		host_buffers.push_back(*g_buffers.get<Buffer>(NameTable::get_index("host sion axe vertices")));
		host_buffers.push_back(*g_buffers.get<Buffer>(NameTable::get_index("host sion axe indices")));
		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; i++) {
			std::string num = std::to_string(i + 1);
			host_buffers.push_back(*g_buffers.get<Buffer>(NameTable::get_index((name + num).c_str())));
		}

		g_host_memory = HostMemory(host_buffers);

		std::vector<Buffer> device_buffers;
		device_buffers.reserve(2);
		device_buffers.push_back(*g_buffers.get<Buffer>(NameTable::get_index("device sion axe vertices")));
		device_buffers.push_back(*g_buffers.get<Buffer>(NameTable::get_index("device sion axe indices")));

		std::vector<Image> device_images;
		device_images.reserve(1);
		device_images.push_back(*g_resources.get<Texture>(NameTable::get_index("sion axe texture")));

		g_device_memory = DeviceMemory(device_buffers, device_images);
	}
}