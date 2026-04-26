#pragma once

#include <cstdint>

namespace Engine {
	class Entity;

	class Component {
		public:
		static uint32_t nextTypeID;
		template<class T>
		static uint32_t getTypeID() {
			static uint32_t typeID = nextTypeID++;
			return typeID;
		}

		protected:
		explicit Component(Entity* entity) : entity{ entity } {}
		Entity* entity{};

		public:
		virtual ~Component() = default;
		void setEntity(Entity* entity) { this->entity = entity; }
		Entity* getEntity() const { return entity; }
	};
}