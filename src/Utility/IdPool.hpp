#pragma once

#include <unordered_map>
#include <memory>
#include <type_traits>
#include "Engine/ComponentTypeId.hpp"

template<class Id, class T>
class IdPool {
	private:
	std::unordered_map<Id, std::unique_ptr<T>> t_map{};

	public:
	template<class TDerived, class... Args>
	void add(Id map_id, Args&&... args) {
		static_assert(std::is_base_of_v<T, TDerived>, "add: TDerived must inherit from T!");
		
		auto i = t_map.find(map_id);

		if(i == t_map.end()) {
			t_map[map_id] = std::make_unique<TDerived>(std::forward<Args>(args)...);
		}
	}

	template<class TDerived>
	TDerived* get(Id map_id) {
		static_assert(std::is_base_of_v<T, TDerived>, "get: TDerived must inherit from T!");

		auto i = t_map.find(map_id);

		return (i != t_map.end()) ? static_cast<TDerived*>(i->second.get()) : nullptr;
	}

	bool remove(Id map_id) {
		return t_map.erase(map_id) > 0;
	}
};

class Component;
using Entity = IdPool<ComponentTypeId, Component>;