#pragma once

#include "Util.h"

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

		enum class State {
			UNINITIALIZED,
			INITIALIZING,
			INITIALIZED,
			DESTROYING,
			DESTROYED
		};

		protected:
		explicit Component(Entity* const& ENTITY) : entity{ ENTITY }, state{ State::UNINITIALIZED } {}

		Entity* entity{};
		State state{};

		public:
		virtual ~Component() = default;
		bool isInitialized() const { return state == State::INITIALIZED; }
		void setEntity(Entity* const& ENTITY) { this->entity = ENTITY; }
		Entity* getEntity() const { return entity; }
	};
}