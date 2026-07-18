#include <cstdint>
#include <vector>
#include "Utility/IdPool.hpp"
#include "ShaderStructs/PBRVertex.hpp"
#include "HostMemory.hpp"
#include "DeviceMemory.hpp"
#include "Wrappers/Buffer.hpp"
#include "Wrappers/Image.hpp"
#include "Wrappers/DescriptorSet.hpp"
#include "Wrappers/ImageView.hpp"
#include "Wrappers/Sampler.hpp"

namespace MemoryManager {
	inline IdPool<Buffer> g_buffers;
	inline IdPool<Image> g_images;
	inline IdPool<ImageView> g_image_views;
	inline IdPool<DescriptorSet> g_descriptor_sets;
	inline IdPool<Sampler> g_samplers;

	inline std::vector<PBRVertex> g_vertices;
	inline std::vector<uint32_t> g_indices;

	inline std::vector<glm::vec3> g_simple_vertices;
	inline std::vector<uint32_t> g_simple_indices;

	inline HostMemory g_host_memory;
	inline DeviceMemory g_device_memory_1;
	inline DeviceMemory g_device_memory_2;

	void init();
	void destroy();

	void load_vertices_and_indices();
	void init_buffers();
	void init_images();
	void init_memory();
	void populate_memory();
	void init_image_views();
	void init_samplers();
	void init_descriptor_sets();
	void write_descriptor_sets();
}