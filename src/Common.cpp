#include <algorithm>
#include <fstream>
#include "Common.h"

namespace Common {
    bool fContainsAll(std::vector<std::string> const& big, std::vector<std::string> const& small) {
        std::vector<std::string> bigCopy(big);
        std::vector<std::string> smallCopy(small);

        std::sort(bigCopy.begin(), bigCopy.end());
        std::sort(smallCopy.begin(), smallCopy.end());

        return std::ranges::includes(bigCopy, smallCopy);
    }

	[[nodiscard]] const std::vector<char> fLoadSprivFileBytes(std::string const& PATH) {
		std::ifstream fileIn(PATH, std::ios::binary | std::ios::ate);
		if(!fileIn.good()) {
			throw std::runtime_error("Failure reading file at " + std::string(PATH));
		}

		uint32_t fileSize = fileIn.tellg();
		std::vector<char> bytes(fileSize);

		fileIn.seekg(0);
		fileIn.read(bytes.data(), fileSize);

		return bytes;
	}
}