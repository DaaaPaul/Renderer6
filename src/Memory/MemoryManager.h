#include "Geometry/Vertex.hpp"
#include "HostMemory.hpp"
#include "DeviceMemory.hpp"
#include "Resource.hpp"
#include "Buffer.hpp"
#include "DepthImage.hpp"
#include "DescriptorSet.hpp"
#include "ImageView.hpp"

namespace MemoryManager {
	inline const char* texture_name = "sion texture";
	inline const char* host_vertices_name = "host sion axe vertices";
	inline const char* host_indices_name = "host sion axe indices";
	inline const char* device_vertices_name = "device sion axe vertices";
	inline const char* device_indices_name = "device sion axe indices";

	inline std::vector<const char*> transform_matrices = {
		"transform matrices 0", 
		"transform matrices 1",
		"transform matrices 2",
		"transform matrices 3"
	};

	inline Resources g_resources;
	inline ImageView g_texture_image_view;
	inline Buffers g_buffers;
	inline DepthImage g_depth_image;
	inline ImageView g_depth_image_view;
	inline DescriptorSet g_descriptor_set;
	inline VkSampler g_sampler{};

	inline std::vector<Vertex> g_vertices;
	inline std::vector<uint32_t> g_indices;

	inline HostMemory g_host_memory;
	inline DeviceMemory g_device_memory;

	void init();
}