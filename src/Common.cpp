#include <algorithm>
#include <fstream>
#include "Common.h"

namespace Common {
    bool containsAll(std::vector<std::string> const& BIG, std::vector<std::string> const& SMALL) {
        std::vector<std::string> bigCopy(BIG);
        std::vector<std::string> smallCopy(SMALL);

        std::sort(bigCopy.begin(), bigCopy.end());
        std::sort(smallCopy.begin(), smallCopy.end());

        return std::ranges::includes(bigCopy, smallCopy);
    }

	[[nodiscard]] const std::vector<char> loadSprivFileBytes(std::string const& PATH) {
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