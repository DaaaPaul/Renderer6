#pragma once

#include <cstdint>

class Entity;

class Component {
	public:
	static uint32_t next_type_id;
	template<class T>
	static uint32_t get_type_id() {
		static uint32_t type_id = next_type_id++;
		return type_id;
	}

	protected:
	explicit Component(Entity* entity) : entity{ entity } {}
	Entity* entity{};

	public:
	virtual ~Component() = default;
	void set_entity(Entity* entity) { this->entity = entity; }
	Entity* get_entity() const { return entity; }
};
