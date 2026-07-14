#pragma once

#include <cstdint>
#include "Utility/IdPool.hpp"

class Component {
	protected:
	explicit Component(Entity* entity) : entity{ entity } {
	
	}
	Entity* entity{};

	public:
	virtual ~Component() = default;

	void set_entity(Entity* entity) { 
		this->entity = entity; 
	}

	Entity* get_entity() const { 
		return entity; 
	}
};
