#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>

#define DELETE_COPY_CTOR(x) \
	x(const x&) = delete;

#define DELETE_COPY_ASSIGN(x) \
	x& operator=(const x&) = delete;

#define DELETE_MOVE_CTOR(x) \
	x(x&&) = delete;

#define DELETE_MOVE_ASSIGN(x) \
	x& operator=(x&&) = delete;

#define DELETE_COPYING(x) \
	DELETE_COPY_CTOR(x) \
	DELETE_COPY_ASSIGN(x)

#define DELETE_MOVING(x) \
	DELETE_MOVE_CTOR(x) \
	DELETE_MOVE_ASSIGN(x)

#define DELETE_COPYING_MOVING(x) \
	DELETE_COPYING(x) \
	DELETE_MOVING(x)

std::ostream& operator<<(std::ostream& os, glm::vec2 vec2);
std::ostream& operator<<(std::ostream& os, glm::vec3 vec3);
std::ostream& operator<<(std::ostream& os, glm::vec4 vec4);
std::ostream& operator<<(std::ostream& os, glm::mat4 mat4);

namespace Utility {
	inline constexpr float PI = 3.14159274f;
	inline constexpr uint32_t INVALID_UINT32 = 0xFFFFFFFF;

	inline void println(const char* msg) {
		std::cout << msg << '\n';
	}

	inline void println(std::string msg) {
		std::cout << msg << '\n';
	}

	inline void println(glm::mat4 mat4) {
		std::cout << mat4 << '\n';
	}

	inline void check_nullptr(void* p, const char* error_msg = "utility::check_nullptr: failed") {
		if(!p) {
			throw std::runtime_error(error_msg);
		}
	}

	inline void check_true(bool b, const char* error_msg = "utility::check_true: failed") {
		if(!b) {
			throw std::runtime_error(error_msg);
		}
	}

	std::vector<std::string> to_string(const std::vector<const char*>* c_strings);

	bool contains_all(std::vector<std::string> these_strings, std::vector<std::string> contain_these);

	float random();

	std::vector<char> get_file_bytes(std::string file_path);

	glm::vec2 get_circle_position(float angle, float radius);

	std::pair<std::vector<glm::vec3>,  std::vector<uint32_t>> get_sphere(float radius, uint32_t stack_count, uint32_t sector_count);
}