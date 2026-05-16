#pragma once

#include <cassert>
#include "Utility.h"

// variadic class template declaration
// goal is just to give leeway for the existance of below 2 specializations
template<class... Ts>
struct FeatureChain; 

// full specialization of the empty FeatureChain
template<>
struct FeatureChain<> {
	FeatureChain() = default;
	FeatureChain(const FeatureChain<>&) = default;
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

	FeatureChain(const FeatureChain<First, Rest...>& feature_chain) :
		FeatureChain<Rest...>(feature_chain),
		feature{ feature_chain.feature } {
		set_pointers();
	}

	void set_pointers() {
		if constexpr (sizeof...(Rest) != 0) {
			feature.pNext = &(FeatureChain<Rest...>::feature);
			FeatureChain<Rest...>::set_pointers();
		}
	}

	bool has_all(const FeatureChain<First, Rest...>& REQUIREMENTS) {
		if constexpr (sizeof...(Rest) != 0) {
			return feature_struct_contains_all(feature, REQUIREMENTS.feature) && FeatureChain<Rest...>::has_all(REQUIREMENTS);
		} else {
			return feature_struct_contains_all(feature, REQUIREMENTS.feature);
		}
	}

	template<class F>
	static bool feature_struct_contains_all(const F& features, const F& features_to_check) {
		bool result = true;

		uint32_t bool_offset = offsetof(F, pNext) + sizeof(void*);
		uint32_t bool_count = (sizeof(F) - bool_offset) / sizeof(VkBool32) - 1;

		const char* p_features_small = reinterpret_cast<const char*>(&features);
		p_features_small += bool_offset;
		const VkBool32* p_features = reinterpret_cast<const VkBool32*>(p_features_small);

		const char* p_check_small = reinterpret_cast<const char*>(&features_to_check);
		p_check_small += bool_offset;
		const VkBool32* p_check = reinterpret_cast<const VkBool32*>(p_check_small);
		
		for(int i = 0; i < bool_count && result; ++i) {
			if(*p_features == VK_FALSE && *p_check == VK_TRUE) {
				result = false;
			} else {
				p_features++;
				p_check++;
			}
		}

		return result;
	}
};