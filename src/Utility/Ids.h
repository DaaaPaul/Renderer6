#pragma once

#include <array>
#include <unordered_map>
#include <memory>
#include "Backend/Swapchain.h"

namespace Ids {
	inline constexpr uint32_t g_SION_TEXTURE = 0;
	inline constexpr uint32_t g_VERTEX_STAGE = 1;
	inline constexpr uint32_t g_INDEX_STAGE = 2;
	inline constexpr uint32_t g_VERTEX_BUFFER = 3;
	inline constexpr uint32_t g_INDEX_BUFFER = 4;

	inline constexpr std::array<uint32_t, Swapchain::g_IMAGE_COUNT> g_UNIFORM_BUFFERS{
		5, 6, 7, 8
	};

	inline constexpr uint32_t g_SION_TEXTURE_VIEW = 9;
	inline constexpr uint32_t g_DEPTH_IMAGE = 10;
	inline constexpr uint32_t g_DEPTH_VIEW = 11;

	inline constexpr uint32_t g_DESCRIPTOR_SET = 12;
	inline constexpr uint32_t g_SAMPLER = 13;
}