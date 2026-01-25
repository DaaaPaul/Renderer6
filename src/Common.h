#include <stdexcept>

#define CHECK_NULLPTR(ptr) \
    if(!ptr) { \
        throw std::runtime_error(#ptr "is a nullptr"); \
    }