#pragma once

#include <cassert>
#include "Util.h"

// variadic class template declaration
// goal is just to give leeway for the existance of below 2 specializations
template<class... Ts>
struct FeatureChain; 

// full specialization of the empty FeatureChain
template<>
struct FeatureChain<> {
	FeatureChain() = default;
};

// partial specialization of FeatureChain with 1 or more template type parameters
template<class First, class... Rest>
struct FeatureChain<First, Rest...> : FeatureChain<Rest...> {
	First feature{};

	FeatureChain() : feature{} {}

	FeatureChain(First first, Rest... rest) :
		FeatureChain<Rest...>(rest...),
		feature{ first } {
		if constexpr (sizeof...(Rest) != 0) {
			feature.pNext = &(FeatureChain<Rest...>::feature);
		} else {
			feature.pNext = nullptr; 
		}
	}

	void reroutePointers() noexcept {
		if constexpr (sizeof...(Rest) != 0) {
			feature.pNext = &(FeatureChain<Rest...>::feature);
			FeatureChain<Rest...>::reroutePointers();
		}
	}

	// WARNING: must call reroutePointers() after initializing with getShell()
	FeatureChain<First, Rest...> getShell(bool const& PLEDGE_TO_REROUTE) noexcept {
		assert(PLEDGE_TO_REROUTE && "You must reroute after initializing with getShell()! Try again...");
		FeatureChain<First, Rest...> shell{};
		return shell;
	}

	bool hasAllOf(FeatureChain<First, Rest...> const& REQUIREMENTS) {
		if constexpr (sizeof...(Rest) != 0) {
			return Util::checkFeatureHasAll(feature, REQUIREMENTS.feature) && FeatureChain<Rest...>::hasAllOf(REQUIREMENTS);
		} else {
			return Util::checkFeatureHasAll(feature, REQUIREMENTS.feature);
		}
	}
};