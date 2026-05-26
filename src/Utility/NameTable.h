#pragma once

#include <array>
#include <unordered_map>
#include <cassert>
#include "Backend/Swapchain.h"

namespace NameTable {
	inline constexpr uint16_t g_SION_TEXTURE = 0;
	inline constexpr uint16_t g_VERTEX_STAGE = 1;
	inline constexpr uint16_t g_INDEX_STAGE = 2;
	inline constexpr uint16_t g_VERTEX_BUFFER = 3;
	inline constexpr uint16_t g_INDEX_BUFFER = 4;

	inline constexpr std::array<uint16_t, Swapchain::g_IMAGE_COUNT> g_TRANSFORM_MATRICES{
		5, 6, 7, 8
	};

	inline constexpr std::array<uint16_t, UINT16_MAX> g_NAMES{
		g_SION_TEXTURE,
		g_VERTEX_STAGE,
		g_INDEX_STAGE,
		g_VERTEX_BUFFER,
		g_INDEX_BUFFER,
		g_TRANSFORM_MATRICES[0], g_TRANSFORM_MATRICES[1], g_TRANSFORM_MATRICES[2], g_TRANSFORM_MATRICES[3]
	};
}