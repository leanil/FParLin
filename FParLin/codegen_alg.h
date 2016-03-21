#pragma once

#include "expr.h"
#include <string>

struct codegen_alg {
	codegen_alg(int threshold = 0) : threshold{ threshold } {}
	string operator()(F<string>);

	int threshold;
};
