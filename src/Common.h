#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>

#define CHECK_NULLPTR(ptr) \
    if(!ptr) { \
        throw std::runtime_error(#ptr " is a nullptr"); \
    }

#define CHECK_VK_SUCCESS(vkCreateCmd) \
	if(vkCreateCmd != VK_SUCCESS) { \
		std::string errorMsg = #vkCreateCmd " did not return VK_SUCCESS, instead returning " + vkCreateCmd; \
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

#define STATIC_CAST_VECTOR_SIZE(vector) \
	static_cast<uint32_t>(vector.size())

namespace Common {
	bool containsAll(std::vector<std::string> const& big, std::vector<std::string> const& small);
}