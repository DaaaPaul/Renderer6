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
			return featuresContainsAll(feature, REQUIREMENTS.feature) && FeatureChain<Rest...>::hasAllOf(REQUIREMENTS);
		} else {
			return featuresContainsAll(feature, REQUIREMENTS.feature);
		}
	}

	template<class F>
	static bool featuresContainsAll(F const& HAVE, F const& CHECK) {
		bool hasAll = true;

		uint32_t firstVkBool32Offset = offsetof(F, pNext) + sizeof(void*);
		uint32_t howManyVkBool32 = (sizeof(F) - firstVkBool32Offset) / sizeof(VkBool32) - 1;

		char const* SNIPER_HAVE = reinterpret_cast<char const*>(&HAVE);
		SNIPER_HAVE += firstVkBool32Offset;
		VkBool32 const* BOOL_SNIPER_HAVE = reinterpret_cast<VkBool32 const*>(SNIPER_HAVE);

		const char* SNIPER_CHECK = reinterpret_cast<char const*>(&CHECK);
		SNIPER_CHECK += firstVkBool32Offset;
		VkBool32 const* BOOL_SNIPER_CHECK = reinterpret_cast<VkBool32 const*>(SNIPER_CHECK);
		
		for(int i = 0; i < howManyVkBool32 && hasAll; ++i) {
			if(*BOOL_SNIPER_HAVE == VK_FALSE && *BOOL_SNIPER_CHECK == VK_TRUE) {
				hasAll = false;
			} else {
				BOOL_SNIPER_HAVE++;
				BOOL_SNIPER_CHECK++;
			}
		}

		return hasAll;
	}
};