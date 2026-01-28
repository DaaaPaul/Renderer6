#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>

#define CHECK_NULLPTR(ptr) \
    if(!ptr) { \
        throw std::runtime_error(#ptr " is a nullptr"); \
    }

#define CHECK_BOOL(b) \
	if(!b) { \
        throw std::runtime_error(#b " is false"); \
	}

#define CHECK_VK_SUCCESS(vkCreateCmd) \
	if(vkCreateCmd != VK_SUCCESS) { \
		std::string errorMsg = #vkCreateCmd " did not return VK_SUCCESS, instead returning " + std::to_string(static_cast<int>(vkCreateCmd)); \
        throw std::runtime_error(errorMsg); \
	}

#define CHECK_CONTAINS_ALL(big, small) \
	if(!Common::containsAll(big, small)) { \
		throw std::runtime_error(#big " does not contain all of " #small); \
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

namespace Common {
	bool containsAll(std::vector<std::string> const& big, std::vector<std::string> const& small);
}