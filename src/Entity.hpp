#pragma once

#include <vector>
#include <type_traits>
#include <memory>
#include <unordered_map>
#include "Component.hpp"

class Entity {
	std::vector<std::unique_ptr<Component>> components{};
	std::unordered_map<uint32_t, Component*> component_map{};

	public:
	template<class T, class... Args>
	T* add_component(Args&&... args) {
		static_assert(std::is_base_of<Component, T>::value, "T needs to be a Component");
		T* p_component = nullptr;

		uint32_t type_id = Component::get_type_id<T>();
		auto i = component_map.find(type_id);

		if(i != component_map.end()) {
			p_component = static_cast<T*>(i->second);
		} else {
			std::unique_ptr<T> component = std::make_unique<T>(this, std::forward<Args>(args)...);

			p_component = component.get();
			component_map[type_id] = p_component;

			components.push_back(std::move(component));
		}

		return p_component;
	}

	template<class T>
	T* get_component() {
		T* p_component = nullptr;

		auto i = component_map.find(Component::get_type_id<T>());

		if(i != component_map.end()) {
			p_component = static_cast<T*>(i->second);
		}

		return p_component;
	}

	template<class T>
	bool remove_component() {
		T* p_component = nullptr;
		auto i = component_map.find(Component::get_type_id<T>());

		if(i != component_map.end()) {
			p_component = static_cast<T*>(i->second);
			component_map.erase(i);

			for(auto i2 = components.begin(); i2 < components.end(); ++i2) {
				if(i2->get() == p_component) {
					components.erase(i2);
					break;
				}
			}
		}

		return p_component != nullptr;
	}
};
