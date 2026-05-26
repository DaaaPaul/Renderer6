#include "Utility/NameTable.h"

//namespace NameTable {
//	uint32_t push_name(std::string_view name) {
//		assert(g_next_index < g_NAME_COUNT);
//
//		uint32_t index = g_next_index;
//
//		g_names[index] = name;
//
//		g_lookup[name] = index;
//
//		g_next_index++;
//
//		return index;
//	}
//
//	uint32_t get_index(std::string_view name) {
//		uint32_t index = UINT32_MAX;
//
//		auto i = g_lookup.find(name);
//
//		if(i != g_lookup.end()) {
//			index = i->second;
//		}
//
//		return index;
//	}
//}