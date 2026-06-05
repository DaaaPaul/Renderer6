#pragma once

#include <cstdint>
#include "Utility/IdPool.hpp"

class Component {
	protected:
	explicit Component(IdPool<Component>* entity) : entity{ entity } {
	
	}
	IdPool<Component>* entity{};

	public:
	virtual ~Component() = default;

	void set_entity(IdPool<Component>* entity) { 
		this->entity = entity; 
	}

	IdPool<Component>* get_entity() const { 
		return entity; 
	}
};
