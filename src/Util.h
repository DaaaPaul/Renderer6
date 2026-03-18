#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <string>
#include <vector>

#define UINT32(vecSize) \
	static_cast<uint32_t>(vecSize)

#define CHECK_NULLPTR(ptr, errMsg) \
    if(!ptr) { \
        throw std::runtime_error(errMsg); \
    }

#define CHECK_BOOL(b, errMsg) \
	if(!b) { \
        throw std::runtime_error(errMsg); \
	}

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
	[[nodiscard]] bool containsAll(std::vector<std::string> const& BIG, std::vector<std::string> const& SMALL);
	[[nodiscard]] std::vector<char> getFileBytes(std::string const& PATH);
	[[nodiscard]] float random() noexcept;
}

using VkLogicalDevice = VkDevice;