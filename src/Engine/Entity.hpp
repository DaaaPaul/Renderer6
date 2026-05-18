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
	template<class ComponentType, class... Args>
	ComponentType* add_component(Args&&... args) {
		static_assert(std::is_base_of<Component, ComponentType>::value, "ComponentType needs to be a Component");

		ComponentType* p_component = nullptr;

		uint32_t type_id = Component::get_type_id<ComponentType>();
		auto i = component_map.find(type_id);

		if(i != component_map.end()) {
			p_component = static_cast<ComponentType*>(i->second);
		} else {
			std::unique_ptr<ComponentType> component = std::make_unique<ComponentType>(this, std::forward<Args>(args)...);

			p_component = component.get();
			component_map[type_id] = p_component;

			components.push_back(std::move(component));
		}

		return p_component;
	}

	template<class ComponentType>
	ComponentType* get_component() {
		static_assert(std::is_base_of<Component, ComponentType>::value, "ComponentType needs to be a Component");

		ComponentType* p_component = nullptr;

		auto i = component_map.find(Component::get_type_id<ComponentType>());

		if(i != component_map.end()) {
			p_component = static_cast<ComponentType*>(i->second);
		}

		return p_component;
	}

	template<class ComponentType>
	bool remove_component() {
		static_assert(std::is_base_of<Component, ComponentType>::value, "ComponentType needs to be a Component");

		ComponentType* p_component = nullptr;

		auto i = component_map.find(Component::get_type_id<ComponentType>());

		if(i != component_map.end()) {
			p_component = static_cast<ComponentType*>(i->second);
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
