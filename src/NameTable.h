#pragma once

#include <array>
#include <unordered_map>
#include <cassert>

namespace NameTable {
	inline constexpr uint32_t NUM_ENTRIES = 1024;

	inline std::array<const char*, NUM_ENTRIES> names{};
	inline std::unordered_map<const char*, uint32_t> lookup{};
	inline uint32_t nextIdx = 0;

	uint32_t pushEntry(const char* name);

	uint32_t getIdx(const char* name);

	constexpr const char* getName(uint32_t idx);
}