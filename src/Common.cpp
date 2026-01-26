#include "Common.h"
#include <algorithm>

bool containsAll(std::vector<std::string> const& big, std::vector<std::string> const& small) {
    std::vector<std::string> bigCopy(big);
    std::vector<std::string> smallCopy(small);

    std::sort(bigCopy.begin(), bigCopy.end());
    std::sort(smallCopy.begin(), smallCopy.end());

    return std::ranges::includes(bigCopy, smallCopy);
}