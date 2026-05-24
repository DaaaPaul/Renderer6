#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>

class Resource {
	private:
	uint32_t name_index{};
	uint32_t user_count{};

	protected:
	explicit Resource(uint32_t name_index) : name_index{ name_index }, user_count{ 1 } {}

	public:
	virtual ~Resource() = default;

	uint32_t get_name_index() const { 
		return name_index; 
	}

	void inc_user_count() { 
		++user_count; 
	};

	void dec_user_count() { 
		if(user_count > 0) --user_count; 
	};

	uint32_t get_user_count() const { 
		return user_count; 
	}
};

class Resources {
	private:
	std::unordered_map<uint32_t, std::unique_ptr<Resource>> resource_map{};

	public:
	template<class ResourceType, class... Args>
	ResourceType* add(uint32_t name_index, Args&&... args) {
		static_assert(std::is_base_of<Resource, ResourceType>::value, "ResourceType needs to inherit Resource");
		ResourceType* p_resource = nullptr;

		auto i = resource_map.find(name_index);

		if(i != resource_map.end()) {
			i->second->inc_user_count();
			p_resource = static_cast<ResourceType*>(i->second.get());
		} else {
			resource_map[name_index] = std::make_unique<ResourceType>(name_index, std::forward<Args>(args)...);
			p_resource = static_cast<ResourceType*>(resource_map.at(name_index).get());
		}

		return p_resource;
	}

	template<class ResourceType>
	ResourceType* get(uint32_t name_index) {
		static_assert(std::is_base_of<Resource, ResourceType>::value, "ResourceType needs to inherit Resource");

		ResourceType* p_resource = nullptr;

		auto i = resource_map.find(name_index);

		if(i != resource_map.end()) {
			p_resource = static_cast<ResourceType*>(i->second.get());
		}

		return p_resource;
	}

	bool remove(uint32_t name_index) {
		bool remove_success = false;
		auto i = resource_map.find(name_index);

		if(i != resource_map.end()) {
			i->second->dec_user_count();

			if(i->second->get_user_count() == 0) {
				resource_map.erase(i);
			}

			remove_success = true;
		}

		return remove_success;
	}
};
