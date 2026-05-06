#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>

class Resource {
	private:
	uint32_t nameIdx{};
	uint32_t users{};

	protected:
	explicit Resource(uint32_t idx) : nameIdx(idx), users{ 1 } {}
	virtual ~Resource() = default;

	public:
	void setNameIdx(uint32_t idx) { nameIdx = idx; }
	uint32_t getNameIdx() const { return nameIdx; }

	void incrementUsers() { ++users; };
	void decrementUsers() { if(users > 0) --users; };
	uint32_t getUsers() const { return users; }
};

class Resources {
	private:
	std::unordered_map<uint32_t, std::unique_ptr<Resource>> resources{};

	public:
	template<class T, class... Args>
	T* useResource(uint32_t idx, Args&&... args) {
		static_assert(std::is_base_of<Resource, T>::value, "T needs to be a Resource");
		T* resourcePtr = nullptr;

		auto i = resources.find(idx);

		if(i != resources.end()) {
			i->second->incrementUsers();
			resourcePtr = static_cast<T*>(i->second.get());
		} else {
			resources[idx] = std::make_unique<T>(idx, std::forward<Args>(args)...);
			resourcePtr = static_cast<T*>(resources[idx].get());
		}

		return resourcePtr;
	}

	template<class T>
	bool removeResource(uint32_t idx) {
		bool removeSuccess = false;
		auto i = resources.find(idx);

		if(i != resources.end()) {
			i->second->decrementUsers();

			if(i->second->getUsers() == 0) {
				resources.erase(i);
			}

			removeSuccess = true;
		}

		return removeSuccess;
	}
};
