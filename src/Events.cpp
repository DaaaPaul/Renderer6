#include "Events.hpp"

namespace Engine {
	void Events::addListener(EventListener* l) {
		listeners.push_back(l);
	}

	bool Events::removeListener(EventListener* l) {
		bool removeSuccess = false;

		for(auto i = listeners.begin(); i < listeners.end() && !removeSuccess; i++) {
			if(*i == l) {
				listeners.erase(i);
				removeSuccess = true;
			}
		}

		return removeSuccess;
	}

	void Events::dispatch(Event const& E) const {
		for(EventListener* l : listeners) {
			l->onEvent(E);
		}
	}
}