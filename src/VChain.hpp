#pragma once

template<class... Ts>
struct VChain;

template<>
struct VChain<> {
	VChain() = default;
};

template<class First, class... Rest>
struct VChain<First, Rest...> : VChain<Rest...> {
	First val{};

	VChain(First first, Rest... rest) :
		VChain<Rest...>(rest...),
		val{ first } {
		if constexpr (sizeof...(Rest) > 0) {
			val.pNext = &(VChain<Rest...>::val);
		} else {
			val.pNext = nullptr; 
		}
	}

	void reroutePointers() noexcept {
		if constexpr (sizeof...(Rest) > 0) {
			val.pNext = &(VChain<Rest...>::val);
			VChain<Rest...>::reroutePointers();
		}
	}
};