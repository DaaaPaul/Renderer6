#include "NameTable.h"

namespace NameTable {
	uint32_t pushEntry(const char* name) {
		assert(nextIdx < NUM_ENTRIES);

		uint32_t idx = nextIdx;
		names[idx] = name;

		lookup[name] = idx;

		nextIdx++;

		return idx;
	}

	uint32_t getIdx(const char* name) {
		uint32_t idx = UINT32_MAX; // return if name wasn't found 
		auto i = lookup.find(name);

		if(i != lookup.end()) {
			idx = i->second;
		}

		return idx;
	}

	constexpr const char* getName(uint32_t idx) {
		assert(idx < nextIdx);

		return names[idx];
	}
}