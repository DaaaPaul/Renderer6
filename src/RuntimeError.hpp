#pragma once

#include <stdexcept>

struct RuntimeError : public std::runtime_error {
	bool error{};

	explicit RuntimeError() :
		std::runtime_error(nullptr), error{ false } {}

	explicit RuntimeError(const char* msg) :
		std::runtime_error(msg), error{ true } {}

	void throw_if_error() const {
		if(error) {
			throw *this;
		}
	}
};