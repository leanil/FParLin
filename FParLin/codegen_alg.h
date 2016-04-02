#pragma once

#include "expr.h"
#include <utility>
#include <string>

using codegen_t = std::pair<std::string, bool>;

struct codegen_alg {
	codegen_alg(int threshold = 0) : threshold{ threshold } {}
	codegen_t operator()(F<codegen_t>);

	int threshold;
};
