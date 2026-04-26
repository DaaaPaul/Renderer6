#pragma once

#include <vector>
#include <type_traits>
#include <memory>
#include <unordered_map>
#include "Component.hpp"

namespace Engine {
	class Entity {
		std::vector<std::unique_ptr<Component>> components{};
		std::unordered_map<uint32_t, Component*> componentMap{};

		public:
		template<class T, class... Args>
		T* addComponent(Args&&... args) {
			static_assert(std::is_base_of<Component, T>::value, "T needs to be a Component");
			T* componentPtr = nullptr;

			uint32_t typeID = Component::getTypeID<T>();
			auto i = componentMap.find(typeID);

			if(i != componentMap.end()) {
				componentPtr = static_cast<T*>(i->second);
			} else {
				std::unique_ptr<T> component = std::make_unique<T>(this, std::forward<Args>(args)...);

				componentPtr = component.get();
				componentMap[typeID] = componentPtr;

				components.push_back(std::move(component));
			}

			return componentPtr;
		}

		template<class T>
		T* getComponent() {
			T* componentPtr = nullptr;

			auto i = componentMap.find(Component::getTypeID<T>());

			if(i != componentMap.end()) {
				componentPtr = static_cast<T*>(i->second);
			}

			return componentPtr;
		}

		template<class T>
		bool removeComponent() {
			T* componentPtr = nullptr;
			auto i = componentMap.find(Component::getTypeID<T>());

			if(i != componentMap.end()) {
				componentPtr = static_cast<T*>(i->second);
				componentMap.erase(i);

				for(auto i2 = components.begin(); i2 < components.end(); i2++) {
					if(i2->get() == componentPtr) {
						components.erase(i2);
						break;
					}
				}
			}

			return componentPtr != nullptr;
		}
	};
}