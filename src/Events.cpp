#include "Events.hpp"

namespace Engine {
	EventListener* Events::addListener(EventListener* const& LISTENER) {
		listeners.push_back(LISTENER);
	}

	bool Events::removeListener(EventListener* const& LISTENER) {
		bool removeSuccess = false;

		for(std::vector<EventListener*>::iterator i = listeners.begin(); i < listeners.end() && !removeSuccess; i++) {
			if(*i == LISTENER) {
				listeners.erase(i);
				removeSuccess = true;
			}
		}

		return removeSuccess;
	}

	void Events::dispatch(Event const& EVENT) const {
		for(EventListener* const& listener : listeners) {
			listener->onEvent(EVENT);
		}
	}
}