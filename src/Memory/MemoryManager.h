#include <cstdint>
#include <vector>
#include "Utility/IdPool.hpp"
#include "Geometry/Vertex.hpp"
#include "HostMemory.hpp"
#include "DeviceMemory.hpp"
#include "Buffer.hpp"
#include "Image.hpp"
#include "DescriptorSet.hpp"
#include "ImageView.hpp"
#include "Sampler.hpp"

namespace MemoryManager {
	inline IdPool<uint32_t, Buffer> g_buffers;
	inline IdPool<uint32_t, Image> g_images;
	inline IdPool<uint32_t, ImageView> g_image_views;
	
	inline DescriptorSet g_descriptor_set;
	inline Sampler g_sampler;

	inline std::vector<Vertex> g_vertices;
	inline std::vector<uint32_t> g_indices;

	inline HostMemory g_host_memory;
	inline DeviceMemory g_device_memory_1;
	inline DeviceMemory g_device_memory_2;

	void init();
	void destroy();
}