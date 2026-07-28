#pragma once

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <string>

template<class T>
class Addmap {
	public:
	Addmap() = default;
	explicit Addmap(uint32_t capacity) {
		objects.reserve(capacity);
	}

	template<class... ArgTs>
	void add(std::string name, ArgTs&&... args) {
		name_map[name] = objects.size();

		objects.emplace_back(std::forward<ArgTs>(args)...);
	}

	T* get(std::string name) {
		auto i = name_map.find(name);

		if(i == name_map.end()) {
			throw std::runtime_error("get: name doesn't exist in Addmap");
		}

		return &objects[i->second];
	}

	private:
	std::vector<T> objects;
	std::unordered_map<std::string, uint32_t> name_map;
};