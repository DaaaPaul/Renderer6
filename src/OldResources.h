#pragma once

#include <ktx.h>
#include <cstdint>
#include <vector>
#include "Geometry/Vertex.hpp"

namespace Resources {
	inline std::vector<Vertex> g_model_vertices{};
	inline uint32_t g_vertex_buffer_size = UINT32_MAX;
	inline std::vector<uint32_t> g_model_indices{};
	inline uint32_t g_index_buffer_size = UINT32_MAX;

	inline ktxTexture2* g_texture{};

	void load_model();
}