#include "Utility/NameTable.h"

namespace NameTable {
	uint32_t push_name(const char* name) {
		assert(next_index < NAME_COUNT);

		uint32_t index = next_index;

		names[index] = name;

		lookup[name] = index;

		next_index++;

		return index;
	}

	uint32_t get_index(const char* name) {
		uint32_t index = UINT32_MAX;

		auto i = lookup.find(name);

		if(i != lookup.end()) {
			index = i->second;
		}

		return index;
	}
}