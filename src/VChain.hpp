#pragma once

// variadic class template declaration
// goal is just to give leeway for the existance of below 2 specializations
template<class... Ts>
struct VChain; 

// full specialization of the empty VChain
template<>
struct VChain<> {
	VChain() = default;
};

// partial specialization of VChain with 1 or more template type parameters
template<class First, class... Rest>
struct VChain<First, Rest...> : VChain<Rest...> {
	First val{};

	VChain(First first, Rest... rest) :
		VChain<Rest...>(rest...),
		val{ first } {
		if constexpr (sizeof...(Rest) != 0) {
			val.pNext = &(VChain<Rest...>::val);
		} else {
			val.pNext = nullptr; 
		}
	}

	void reroutePointers() noexcept {
		if constexpr (sizeof...(Rest) != 0) {
			val.pNext = &(VChain<Rest...>::val);
			VChain<Rest...>::reroutePointers();
		}
	}
};