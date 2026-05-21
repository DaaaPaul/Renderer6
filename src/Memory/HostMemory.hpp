#pragma once

#include "Memory.hpp"

class HostMemory : public Memory {
	private:
	std::vector<void*> buffer_maps{};

};