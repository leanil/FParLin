#pragma once

#include "expr.h"
#include <string>
#include <utility>
#include <list>

using typecheck_t = std::pair<Fix<F>, std::list<std::string>>;

typecheck_t typecheck_alg(F<typecheck_t>);
