#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <string>
#include <vector>

#define CHECK_NULLPTR(ptr, errMsg) \
    if(!ptr) { \
        throw std::runtime_error(errMsg); \
    }

#define PTR_TO_DECIMAL(p) \
	reinterpret_cast<uintptr_t>(p)

#define CHECK_BOOL(b, errMsg) \
	if(!b) { \
        throw std::runtime_error(errMsg); \
	}

#define CHECK_VK_SUCCESS(vkCreateCmd, errMsg) \
	if(vkCreateCmd != VK_SUCCESS) { \
		std::string errCode = #vkCreateCmd " did not return VK_SUCCESS, instead returning " + std::to_string(static_cast<int>(vkCreateCmd)) + ". "; \
        throw std::runtime_error(errCode + errMsg); \
	}

#define CHECK_CONTAINS_ALL(big, small, errMsg) \
	if(!Util::containsAll(big, small)) { \
		throw std::runtime_error(errMsg); \
	}

#define DELETE_COPY_CONSTRUCTORS(className) \
	className(className const&) = delete; \
	className& operator=(className const&) = delete;

#define DELETE_MOVE_CONSTRUCTORS(className) \
	className(className&&) = delete; \
	className& operator=(className&&) = delete;

#define UINT32_TO_VK_API_VERSION_CSTR(num) \
	((num == 4194304) ? "VK_API_VERSION_1_0" : \
	(num == 4198400) ? "VK_API_VERSION_1_1" : \
	(num == 4202496) ? "VK_API_VERSION_1_2" : \
	(num == 4206592) ? "VK_API_VERSION_1_3" : \
	(num == 4210688) ? "VK_API_VERSION_1_4" : "Invalid vulkan api version!") \

#define POINTER_SIZE(num) \
	(8 * num)

#define PI 3.14159265358979323846f

namespace Util {
	[[nodiscard]] bool containsAll(std::vector<std::string> const& BIG, std::vector<std::string> const& SMALL);
	[[nodiscard]] std::vector<char> getFileBytes(std::string const& PATH);
	[[nodiscard]] float random() noexcept;
}

using VkLogicalDevice = VkDevice;