#pragma once

#include "expr.h"
#include <list>
#include <map>
#include <string>
#include <utility>

enum class element_type { INT, DOUBLE, VECTOR };

struct State {
	list<string> errors;
	std::map<std::pair<element_type, unsigned>, unsigned> results;
	State& add(const State& other, int mul = 1);
	State& add_vec(std::pair<element_type, unsigned> vec, int count);
};

using typecheck_t = pair<Fix<F>, State>;

typecheck_t typecheck_alg(F<typecheck_t>);
