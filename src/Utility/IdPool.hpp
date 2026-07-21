#pragma once

#include <unordered_map>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <stdexcept>
#include "Utility.h"

template<class T>
class IdPool {
	private:
	std::unordered_map<uint32_t, std::unique_ptr<T>> t_map;

	public:
	template<class TDerived, class... Args>
	void add(uint32_t map_id, Args&&... args) {
		static_assert(std::is_base_of_v<T, TDerived>, "add: TDerived must inherit from T!");
		
		auto i = t_map.find(map_id);

		if(i == t_map.end()) {
			t_map[map_id] = std::make_unique<TDerived>(std::forward<Args>(args)...);
		}
	}

	template<class TDerived>
	TDerived* get(uint32_t map_id) {
		static_assert(std::is_base_of_v<T, TDerived>, "get: TDerived must inherit from T!");

		auto i = t_map.find(map_id);

		return (i != t_map.end()) ? static_cast<TDerived*>(i->second.get()) : nullptr;
	}

	bool remove(uint32_t map_id) {
		return t_map.erase(map_id) > 0;
	}
};

class Component;

template<>
class IdPool<Component> {
	private:
	std::unordered_map<std::type_index, std::unique_ptr<Component>> component_map;

	public:
	template<class ComponentType, class... Args>
	void add(Args&&... args) {
		static_assert(std::is_base_of_v<Component, ComponentType>, "add: ComponentType must inherit from Component!");
		
		std::type_index type_id = typeid(ComponentType);

		auto i = component_map.find(type_id);

		if(i == component_map.end()) {
			component_map[type_id] = std::make_unique<ComponentType>(std::forward<Args>(args)...);
		}
	}

	template<class ComponentType>
	ComponentType* get() {
		static_assert(std::is_base_of_v<Component, ComponentType>, "get: ComponentType must inherit from Component!");

		auto i = component_map.find(typeid(ComponentType));
		
		if(i != component_map.end()) {
			return static_cast<ComponentType*>(i->second.get());
		} else {
			THROW_RUNTIME("get: failed to get ComponentType")
		}
	}

	template<class ComponentType>
	bool remove() {
		static_assert(std::is_base_of_v<Component, ComponentType>, "remove: ComponentType must inherit from Component!");

		return component_map.erase(typeid(ComponentType)) > 0;
	}
};

using Entity = IdPool<Component>;