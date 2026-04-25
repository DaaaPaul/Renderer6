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
		EventListener* addListener(EventListener* const&);
		bool removeListener(EventListener* const&);
		void dispatch(Event const&) const;
	};
}