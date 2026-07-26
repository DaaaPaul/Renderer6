#pragma once

#include <cstdint>
#include <vector>
#include "Utility/IdPool.hpp"
#include "ShaderStructs/PBRVertex.hpp"
#include "Buffer.hpp"
#include "Image.hpp"
#include "DescriptorSet.hpp"
#include "ImageView.hpp"
#include "Sampler.hpp"
#include "KtxTexture.hpp"
#include "Memory.hpp"

namespace MemoryManager {
	inline IdPool<Buffer> g_buffers;
	inline IdPool<Image> g_images;
	inline IdPool<KtxTexture> g_textures;
	inline IdPool<ImageView> g_image_views;
	inline IdPool<DescriptorSet> g_descriptor_sets;
	inline IdPool<Sampler> g_samplers;

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