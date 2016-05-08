#pragma once

#include "expr.h"
#include <string>
#include <utility>
#include <list>

using typecheck_t = pair<Fix<F>, list<string>>;

typecheck_t typecheck_alg(F<typecheck_t>);
