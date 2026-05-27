#include <cstdint>

class ComponentTypeId {
	private:
	uint32_t type_id{};

	ComponentTypeId(uint32_t type_id) : type_id{ type_id } {
	
	}

	static uint32_t next_type_id;

	public:
	template<class ComponentType>
	static ComponentTypeId get() {
		static ComponentTypeId type_id(next_type_id++);

		return type_id;
	}
};