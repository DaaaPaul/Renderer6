#pragma once

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <utility>
#include "Utility.h"

template<class T>
class Slotmap {
	public:
	Slotmap() = default;
	explicit Slotmap(uint32_t capacity) {
		objects.reserve(capacity);
		object_to_slot.reserve(capacity);
		object_to_id.reserve(capacity);
		slot_to_object.reserve(capacity);
	}

	template<class... ArgTs>
	void emplace_with_id(int id, ArgTs&&... args) {
		if(id_to_slot.find(id) != id_to_slot.end()) {
			throw std::runtime_error("emplace_with_id: id already exists in Slotmap");
		}

		uint32_t new_slot_index = new_slot();

		object_to_slot.push_back(new_slot_index);
		object_to_id.push_back(id);
		slot_to_object[new_slot_index] = objects.size();
		id_to_slot.emplace(id, new_slot_index);

		objects.emplace_back(std::forward<ArgTs>(args)...);
	}

	void erase(int id) {
		uint32_t erase_slot = get_slot_index_with_id(id);
		uint32_t erase_object_index = slot_to_object[erase_slot];
		uint32_t back_index = objects.size() - 1;
		uint32_t fresh_slot = object_to_slot[back_index];
		int fresh_id = object_to_id[back_index];

		if(erase_object_index != back_index) {
			std::swap(objects[erase_object_index], objects[back_index]);

			object_to_slot[erase_object_index] = fresh_slot;
			object_to_id[erase_object_index] = fresh_id;
			slot_to_object[fresh_slot] = erase_object_index;
			id_to_slot[fresh_id] = fresh_slot;
		}

		objects.pop_back();
		object_to_slot.pop_back();
		object_to_id.pop_back();

		id_to_slot.erase(id);
		slot_to_object[erase_slot] = Utility::INVALID_UINT32; 
		free.push_back(erase_slot);
	}

	T* get(int id) {
		uint32_t slot_index = get_slot_index_with_id(id);

		if(slot_index >= slot_to_object.size() || slot_to_object[slot_index] == Utility::INVALID_UINT32) {
			throw std::runtime_error("get: invalid slot");
		}

		return &objects[slot_to_object[slot_index]];
	}

	private:
	uint32_t get_slot_index_with_id(int id) {
		auto p_id_to_slot_pair = id_to_slot.find(id);
		
		if(p_id_to_slot_pair == id_to_slot.end()) {
			throw std::runtime_error("get_slot_index_with_id: id doesn't exist in Slotmap");
		}

		return p_id_to_slot_pair->second;
	}

	uint32_t new_slot() {
		uint32_t slot = Utility::INVALID_UINT32;
	
		if(free.size() > 0) {
			slot = free.back();
			free.pop_back();
		} else {
			slot = slot_to_object.size();
			slot_to_object.push_back(Utility::INVALID_UINT32);
		}

		return slot;
	}

	std::vector<T> objects;
	std::vector<uint32_t> object_to_slot;
	std::vector<int> object_to_id;
	std::vector<uint32_t> slot_to_object;
	std::vector<uint32_t> free;
	std::unordered_map<int, uint32_t> id_to_slot;
};