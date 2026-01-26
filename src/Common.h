#include <stdexcept>
#include <string>
#include <vector>

#define CHECK_NULLPTR(ptr) \
    if(!ptr) { \
        throw std::runtime_error(#ptr " is a nullptr"); \
    }

#define CHECK_VK_SUCCESS(vkCreateCmd) \
	if(vkCreateCmd != VK_SUCCESS) { \
        throw std::runtime_error(#vkCreateCmd " did not return VK_SUCCESS"); \
	}

#define CHECK_CONTAINS_ALL(big, small) \
	if(!Common::containsAll(big, small)) { \
		throw std::runtime_error(#big " does not contain all of " #small); \
	}

#define DELETE_COPY_CONSTRUCTORS(class) \
		class(class const&) = delete; \
		class& operator=(class const&) = delete;

#define DELETE_MOVE_CONSTRUCTORS(class) \
		class(class&&) = delete; \
		class& operator=(class&&) = delete;

#define STATIC_CAST_VECTOR_SIZE(vector) \
	static_cast<uint32_t>(vector.size())

namespace Common {
	bool containsAll(std::vector<std::string> const& big, std::vector<std::string> const& small);
}