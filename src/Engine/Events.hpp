#pragma once

#include <vector>

class Event {
	public:
	virtual ~Event() = default;
};

class EventListener {
	public:
	virtual ~EventListener() = default;
	virtual void on_event(const Event& event) = 0;
};

class Events {
	private:
	std::vector<EventListener*> listeners{};

	public:
	void add_listener(EventListener* listener);
	bool remove_listener(EventListener* listener);
	void dispatch(const Event& event) const;
};
