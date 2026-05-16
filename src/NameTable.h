#pragma once

#include <array>
#include <unordered_map>
#include <cassert>

namespace NameTable {
	inline constexpr uint32_t NAME_COUNT = 1024;

	inline std::array<const char*, NAME_COUNT> names{};
	inline std::unordered_map<const char*, uint32_t> lookup{};
	inline uint32_t next_index = 0;

	uint32_t push_name(const char* name);
	uint32_t get_index(const char* name);

	inline const char* get_name(uint32_t index) {
		assert(index < next_index);
		return names[index];
	}
}