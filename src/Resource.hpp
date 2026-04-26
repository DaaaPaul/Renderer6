#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace Engine {
	class Resource {
		private:
		std::string name{};
		uint32_t users{};

		protected:
		explicit Resource(std::string const& N, uint32_t const& U = 1) : name(N), users{ U } {}
		virtual ~Resource() = default;

		public:
		void setName(std::string const& N) { name = N; }
		std::string getName() const { return name; }

		void incrementUsers() { users++; };
		void decrementUsers() { if(users > 0) users--; };
		void setUsers(uint32_t const& U) { users = U; }
		uint32_t getUsers() const { return users; }
	};

	class Resources {
		private:
		std::unordered_map<std::string, std::unique_ptr<Resource>> resources{};

		public:
		template<class T, class... Args>
		T* addResource(std::string const& N, uint32_t const& U = 1, Args&&... args) {
			static_assert(std::is_base_of<Resource, T>::value, "T needs to be a Resource");
			T* resourcePtr = nullptr;

			auto i = resources.find(N);

			if(i != resources.end()) {
				i->second->incrementUsers();
				resourcePtr = static_cast<T*>(i->second.get());
			} else {
				std::unique_ptr<T> resource = std::make_unique<T>(N, U, std::forward<Args>(args)...);
				resourcePtr = resource.get();
				resources[N] = std::move(resource);
			}

			return resourcePtr;
		}

		template<class T>
		T* getResource(std::string const& N) {
			T* resourcePtr = nullptr;

			auto i = resources.find(N);

			if(i != resources.end()) {
				resourcePtr = static_cast<T*>(i->second.get());
			}

			return resourcePtr;
		}

		template<class T>
		bool removeResource(std::string const& N) {
			bool removeSuccess = false;
			auto i = resources.find(N);

			if(i != resources.end()) {
				if(i->second->getUsers() > 1) {
					i->second->decrementUsers();
				} else {
					resources.erase(i);
				}

				removeSuccess = true;
			}

			return removeSuccess;
		}

		bool hasResource(std::string const& N) {
			return resources.find(N) != resources.end();
		}
	};
}