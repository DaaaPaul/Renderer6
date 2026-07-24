#pragma once

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

template<class T>
class SlotMap {
public:
    SlotMap() = default;
    explicit SlotMap(size_t reserveCount) { reserve(reserveCount); }

    void reserve(size_t n) {
        dense.reserve(n);
        dense_to_slot.reserve(n);
        dense_to_id.reserve(n);
        id_to_slot.reserve(n);
    }

    size_t size() const {
		return dense.size(); 
	}

    bool empty() const { 
		return dense.empty(); 
	}

    void clear() {
        dense.clear();
        dense_to_slot.clear();
        dense_to_id.clear();
        id_to_slot.clear();
        slot_to_dense.assign(slot_to_dense.size(), kInvalid);
        free.clear();
    }

    template<class... Args>
    bool emplace_with_id(uint64_t id, Args&&... args) {
        if (id_to_slot.find(id) != id_to_slot.end()) {
			return false;
		}

        const uint32_t slot = alloc_slot();
        const uint32_t dense_index = static_cast<uint32_t>(dense.size());

        dense.emplace_back(std::forward<Args>(args)...);
        dense_to_slot.push_back(slot);
        slot_to_dense[slot] = dense_index;

        dense_to_id.push_back(id);
        id_to_slot.emplace(id, slot);

        return true;
    }

    T* get(uint64_t id) {
        auto it = id_to_slot.find(id);

        if (it == id_to_slot.end()) {
			return nullptr;
		}

        const uint32_t slot = it->second;

        if (slot >= slot_to_dense.size()) {
			return nullptr;
		}

        const uint32_t dense_index = slot_to_dense[slot];

        if (dense_index == kInvalid) {
			return nullptr;
		}

        return &dense[dense_index];
    }

    const T* get(uint64_t id) const { return const_cast<SlotMap*>(this)->get(id); }

    bool contains(uint64_t id) const { return get(id) != nullptr; }

    bool erase(uint64_t id) {
        auto it = id_to_slot.find(id);
        if (it == id_to_slot.end()) return false;

        const uint32_t slot = it->second;
        if (slot >= slot_to_dense.size()) { id_to_slot.erase(it); return false; }
        const uint32_t dense_index = slot_to_dense[slot];
        if (dense_index == kInvalid) { id_to_slot.erase(it); return false; }

        const uint32_t back = (uint32_t)dense.size() - 1;

        if (dense_index != back) {
            std::swap(dense[dense_index], dense[back]);

            const uint32_t movedSlot = dense_to_slot[back];
            dense_to_slot[dense_index] = movedSlot;
            slot_to_dense[movedSlot] = dense_index;

            std::swap(dense_to_id[dense_index], dense_to_id[back]);
            const uint64_t movedId = dense_to_id[dense_index];
            id_to_slot[movedId] = movedSlot;
        }

        dense.pop_back();
        dense_to_slot.pop_back();

        const uint64_t backId = dense_to_id.back();
        dense_to_id.pop_back();
        id_to_slot.erase(backId);

        slot_to_dense[slot] = kInvalid;
        free.push_back(slot);
        return true;
    }

    auto begin() { return dense.begin(); }
    auto end() { return dense.end(); }
    auto begin() const { return dense.begin(); }
    auto end()   const { return dense.end(); }

    T& operator[](size_t denseIndex) { return dense[denseIndex]; }
    const T& operator[](size_t denseIndex) const { return dense[denseIndex]; }

    T* data() { return dense.data(); }
    const T* data() const { return dense.data(); }

    uint64_t id_at(size_t denseIndex) const { return dense_to_id[denseIndex]; }

    uint32_t dense_index_of(uint64_t id) const {
        auto it = id_to_slot.find(id);
        if (it == id_to_slot.end()) return kInvalid;
        const uint32_t slot = it->second;
        if (slot >= slot_to_dense.size()) return kInvalid;
        return slot_to_dense[slot];
    }

    const std::vector<uint64_t>& ids() const { return dense_to_id; }

private:
    static constexpr uint32_t kInvalid = 0xFFFFFFFF;

    std::vector<T>        dense;
    std::vector<uint32_t> dense_to_slot;
    std::vector<uint32_t> slot_to_dense;
    std::vector<uint32_t> free;
    std::vector<uint64_t> dense_to_id;
    std::unordered_map<uint64_t, uint32_t> id_to_slot;

    uint32_t alloc_slot() {
        if (!free.empty()) {
            const uint32_t s = free.back();
            free.pop_back();
            return s;
        }

        const uint32_t s = static_cast<uint32_t>(slot_to_dense.size());
        slot_to_dense.push_back(kInvalid);

        return s;
    }
};