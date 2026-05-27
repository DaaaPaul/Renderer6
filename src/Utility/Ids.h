#pragma once

#include <array>
#include <unordered_map>
#include <memory>
#include "Backend/Swapchain.h"
#include "Engine/Component.hpp"

namespace Ids {
	inline constexpr uint32_t g_SION_TEXTURE = 0;
	inline constexpr uint32_t g_VERTEX_STAGE = 1;
	inline constexpr uint32_t g_INDEX_STAGE = 2;
	inline constexpr uint32_t g_VERTEX_BUFFER = 3;
	inline constexpr uint32_t g_INDEX_BUFFER = 4;

	inline constexpr std::array<uint32_t, Swapchain::g_IMAGE_COUNT> g_TRANSFORM_MATRICES{
		5, 6, 7, 8
	};
}

template<class Id, class T>
class IdPool {
	private:
	std::unordered_map<Id, std::unique_ptr<T>> t_map{};

	template<class TDerived, class... Args>
	void add(Id map_id, Args&&... args) {
		static_assert(std::is_base_of<T, TDerived>::value, "add: TDerived must inherit from T!");
		
		auto i = t_map.find(map_id);

		if(i == t_map.end()) {
			t_map[map_id] = std::make_unique<TDerived>(std::forward<Args>(args)...);
		}
	}

	template<class TDerived>
	TDerived* get(Id map_id) {
		static_assert(std::is_base_of<T, TDerived>::value, "get: TDerived must inherit from T!");

		auto i = t_map.find(map_id);

		return (i != t_map.end()) ? static_cast<TDerived*>(i->second.get()) : nullptr;
	}

	bool remove(Id map_id) {
		return (t_map.find(map_id) != t_map.end()) ? t_map.erase(i), true : false;
	}
};

using Entity = IdPool<ComponentTypeId, Component>;