#pragma once

#include <cmath>
#include <glm/detail/type_vec3.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <string>
#include <vector>

#define PRINTLN(x) std::cout << x << '\n'

#define DA_PI 3.14159274f

namespace Utility {
	inline bool equal(float float1, float float2, float epsilon = 0.001f) {
		return fabs(float1 - float2) <= (((float1 > float2) ? float1 : float2) * epsilon + epsilon);
	}

	inline bool equal(glm::vec3 v1, glm::vec3 v2, float epsilon = 0.001f) {
		return equal(v1.x, v2.x, epsilon) && equal(v1.y, v2.y, epsilon) && equal(v1.z, v2.z, epsilon);
	}

	std::vector<std::string> to_string(const std::vector<const char*>& c_strings);

	bool contains_all(std::vector<std::string> these_strings, std::vector<std::string> contain_these);

	float random();

	std::vector<char> get_file_bytes(const std::string& file_path);
}