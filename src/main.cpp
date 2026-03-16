#include <iostream>
#include <stdexcept>
#include "Engine.h"
#include "Global.h"

int main() {
    try {
		Global::asserts();
		Global::load();
		Global::Engine::asserts();
		Global::Engine::load();
		Engine::run();
	} catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cerr << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}