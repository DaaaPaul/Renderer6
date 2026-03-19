#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <string>
#include <vector>

#define UINT32(vecSize) \
	static_cast<uint32_t>(vecSize)

#define CHECK_VK_SUCCESS(vkCreateCmd, errMsg) \
	if(vkCreateCmd != VK_SUCCESS) { \
		std::string errCode = #vkCreateCmd " did not return VK_SUCCESS, instead returning " + std::to_string(static_cast<int>(vkCreateCmd)) + ". "; \
        throw std::runtime_error(errCode + errMsg); \
	}

#define PTR_TO_DECIMAL(p) \
	reinterpret_cast<uintptr_t>(p)

#define POINTER_SIZE(num) \
	(8 * num)

#define DA_PI 3.14159265358979323846f

namespace Util {
	std::vector<std::string> constCharToString(std::vector<const char*> const&);
	bool containsAll(std::vector<std::string> const& HAVE, std::vector<std::string> const& CHECK);
	std::vector<char> getFileBytes(std::string const& PATH);
	float random() noexcept;
	
	template<class F>
	bool checkFeatureHasAll(F const& HAVE, F const& CHECK) {
		bool hasAll = true;

		uint32_t firstVkBool32Offset = offsetof(F, pNext) + sizeof(void*);
		uint32_t howManyVkBool32 = (sizeof(F) - firstVkBool32Offset) / sizeof(VkBool32) - 1;

		char const* SNIPER_HAVE = reinterpret_cast<char const*>(&HAVE);
		SNIPER_HAVE += firstVkBool32Offset;
		VkBool32 const* BOOL_SNIPER_HAVE = reinterpret_cast<VkBool32 const*>(SNIPER_HAVE);

		const char* SNIPER_CHECK = reinterpret_cast<char const*>(&CHECK);
		SNIPER_CHECK += firstVkBool32Offset;
		VkBool32 const* BOOL_SNIPER_CHECK = reinterpret_cast<VkBool32 const*>(SNIPER_CHECK);
		
		for(int i = 0; i < howManyVkBool32 && hasAll; i++) {
			if(*BOOL_SNIPER_HAVE == VK_FALSE && *BOOL_SNIPER_CHECK == VK_TRUE) {
				hasAll = false;
			} else {
				BOOL_SNIPER_HAVE++;
				BOOL_SNIPER_CHECK++;
			}
		}

		return hasAll;
	}
}

using VkLogicalDevice = VkDevice;