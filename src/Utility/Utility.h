#pragma once

#include <cmath>
#include <glm/detail/type_vec3.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <string>
#include <vector>

#define PRINTLN(x) std::cout << x << '\n'

#define DA_PI 3.14159274f

#define CHECK_NULLPTR(p, error_msg) \
	if(!p) { \
		throw std::runtime_error(error_msg); \
	}

namespace Utility {
	std::vector<std::string> to_string(const std::vector<const char*>& c_strings);

	bool contains_all(std::vector<std::string> these_strings, std::vector<std::string> contain_these);

	float random();

	std::vector<char> get_file_bytes(const std::string& file_path);
}