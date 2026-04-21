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
	FeatureChain(FeatureChain<> const&) = default;
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

	FeatureChain(FeatureChain<First, Rest...> const& OTHER) :
		FeatureChain<Rest...>(OTHER),
		feature{ OTHER.feature } {
		reroutePointers();
	}

	void reroutePointers() {
		if constexpr (sizeof...(Rest) != 0) {
			feature.pNext = &(FeatureChain<Rest...>::feature);
			FeatureChain<Rest...>::reroutePointers();
		}
	}

	bool hasAllOf(FeatureChain<First, Rest...> const& REQUIREMENTS) {
		if constexpr (sizeof...(Rest) != 0) {
			return Util::FeatureChain::checkFeatureHasAll(feature, REQUIREMENTS.feature) && FeatureChain<Rest...>::hasAllOf(REQUIREMENTS);
		} else {
			return Util::FeatureChain::checkFeatureHasAll(feature, REQUIREMENTS.feature);
		}
	}
};