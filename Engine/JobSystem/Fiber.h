#pragma once

#include <array>

using F_ptr = void (*)(void *);
constexpr int MAX_ARGS = 8;

struct Fiber {
	F_ptr fiberFunction;
	std::array<void *, MAX_ARGS> args;

	void *handle;

};