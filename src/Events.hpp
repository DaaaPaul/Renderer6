#pragma once

#include <vector>

namespace Engine {
	class Event {
		public:
		virtual ~Event() = default;
	};

	class EventListener {
		public:
		virtual ~EventListener() = default;
		virtual void onEvent(Event const&) = 0;
	};

	class Events {
		private:
		std::vector<EventListener*> listeners{};

		public:
		void addListener(EventListener*);
		bool removeListener(EventListener*);
		void dispatch(Event const&) const;
	};
}