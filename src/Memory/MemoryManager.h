#pragma once

#include <cstdint>
#include <vector>
#include "containers/Addmap.hpp"
#include "shader/PBRVertex.hpp"
#include "Buffer.hpp"
#include "Image.hpp"
#include "DescriptorSet.hpp"
#include "ImageView.hpp"
#include "Sampler.hpp"
#include "KtxTexture.hpp"
#include "memory.hpp"

namespace MemoryManager {
	inline Addmap<Buffer> g_buffers(12);
	inline Addmap<Image> g_images(1);
	inline Addmap<KtxTexture> g_textures(3);
	inline Addmap<ImageView> g_image_views(4);
	inline Addmap<DescriptorSet> g_descriptor_sets(2);
	inline Addmap<Sampler> g_samplers(1);

	inline std::vector<PBRVertex> g_vertices;
	inline std::vector<uint32_t> g_indices;

	inline std::vector<glm::vec3> g_simple_vertices;
	inline std::vector<uint32_t> g_simple_indices;
	
	inline Memory g_host_memory;
	inline Memory g_device_memory;
	inline Memory g_depth_image_memory;

	void init();
	void destroy();

	void load_vertices_and_indices();
	void init_buffers();
	void init_images();
	void init_textures();
	void init_memory();
	void populate_memory();
	void init_image_views();
	void init_samplers();
	void init_descriptor_sets();
	void write_descriptor_sets();
}