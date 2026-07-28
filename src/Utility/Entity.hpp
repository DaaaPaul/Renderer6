#pragma once

#include <unordered_map>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <stdexcept>
#include "Utility.h"

class Component;

class Entity {
	private:
	std::unordered_map<std::type_index, std::unique_ptr<Component>> component_map;

	public:
	template<class ComponentType, class... Args>
	void add(Args&&... args) {
		static_assert(std::is_base_of_v<Component, ComponentType>, "add: ComponentType must inherit from Component!");
		
		if(component_map.find(typeid(ComponentType)) != component_map.end()) {
			throw std::runtime_error("add: ComponentType already exists in Entity");
		}

		component_map[typeid(ComponentType)] = std::make_unique<ComponentType>(std::forward<Args>(args)...);
	}

	template<class ComponentType>
	ComponentType* get() {
		static_assert(std::is_base_of_v<Component, ComponentType>, "get: ComponentType must inherit from Component!");

		auto i = component_map.find(typeid(ComponentType));
		
		if(i == component_map.end()) {
			throw std::runtime_error("get: failed to get ComponentType");
		}

		return static_cast<ComponentType*>(i->second.get());
	}

	template<class ComponentType>
	void remove() {
		static_assert(std::is_base_of_v<Component, ComponentType>, "remove: ComponentType must inherit from Component!");

		if(!(component_map.erase(typeid(ComponentType)) > 0)) {
			throw std::runtime_error("remove: map_id not found in Entity");
		}
	}
};