#pragma once

#include <cstdint>
#include "Utility/Ids.h"

class ComponentTypeId {
	private:
	uint32_t type_id{};

	ComponentTypeId(uint32_t type_id) : type_id{ type_id } {
	
	}

	static uint32_t next_type_id;

	public:
	template<class ComponentType>
	static ComponentTypeId get() {
		static ComponentTypeId type_id(next_type_id++);

		return type_id;
	}
};

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
