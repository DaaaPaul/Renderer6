#include "Events.hpp"

void Events::add_listener(EventListener* p_listener) {
	listeners.push_back(p_listener);
}

bool Events::remove_listener(EventListener* p_listener) {
	bool removeSuccess = false;

	for(auto i = listeners.begin(); i < listeners.end() && !removeSuccess; ++i) {
		if(*i == p_listener) {
			listeners.erase(i);
			removeSuccess = true;
		}
	}

	return removeSuccess;
}

void Events::dispatch(const Event& event) const {
	for(EventListener* listener : listeners) {
		listener->on_event(event);
	}
}
