#include "Vertex.hpp"
#include "Memory.hpp"

namespace MemoryManager {
	inline std::vector<Texture> g_textures{};
	inline std::vector<DepthImage> g_depth_images{};
	inline std::vector<Buffer> g_device_host_buffers{};
	inline std::vector<Buffer> g_device_buffers{};
	inline std::vector<DescriptorSet> g_descriptor_sets{};

	inline std::vector<Vertex> g_vertices{};
	inline std::vector<uint32_t> g_indices{};

	inline Memory g_device_host_memory{};
	inline Memory g_device_memory{};

	void init();

	std::vector<Texture> get_textures();
	std::vector<DepthImage> get_depth_images();
	std::vector<Buffer> get_device_host_buffers();
	std::vector<DescriptorSet> get_descriptor_sets();
}