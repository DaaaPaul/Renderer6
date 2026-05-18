#include "MemoryManager.h"
#include "Utility/NameTable.h"
#include "Backend/Swapchain.h"
#include "Utility/Utility.h"
#include "Geometry/TransformMatrices.hpp"

namespace MemoryManager {
	void init() {
		g_textures = get_textures();
		g_depth_images = get_depth_images();
		g_device_host_buffers = get_device_host_buffers();
		g_device_buffers = {};
	}

	std::vector<Texture> get_textures() {
		std::vector<Texture> textures;
		textures.reserve(1); // NOTICE

		textures.emplace_back(
			NameTable::push_name("sion axe"),
			R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\textures\Sion_Axe_baseColor.ktx2)"
		);

		return textures;
	}

	std::vector<DepthImage> get_depth_images() {
		std::vector<DepthImage> depth_images;
		depth_images.reserve(1); // NOTICE
		
		depth_images.emplace_back(
			VkExtent3D{Swapchain::g_status.imageExtent.width, Swapchain::g_status.imageExtent.height, 1}
		);

		return depth_images;
	}

	std::vector<Buffer> get_device_host_buffers() {
		std::vector<Buffer> device_host_buffers;
		device_host_buffers.reserve(2 + Swapchain::g_IMAGE_COUNT); // NOTICE

		Utility::load_gltf_model(R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\models\sion axe\scene.gltf)", g_vertices, g_indices);
		
		device_host_buffers.emplace_back(g_vertices.data(), UINT32(g_vertices.size() * sizeof(Vertex)), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		device_host_buffers.emplace_back(g_indices.data(), UINT32(g_indices.size() * sizeof(uint32_t)), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

		for(int i = 0; i < Swapchain::g_IMAGE_COUNT; i++) {
			TransformMatrices* p_empty = new TransformMatrices(glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f));
			device_host_buffers.emplace_back(p_empty, sizeof(TransformMatrices), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		}

		return device_host_buffers;
	}
}