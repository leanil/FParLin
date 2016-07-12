#pragma once

#include "expr.h"
#include <utility>
#include <vector>

using test_t = std::pair<FF, std::vector<double>>;

extern test_t testExpr1; // (2+3)*4 == 20
extern test_t testExpr2; //((λ_x.λ_y.x+y)5)(4*3) == 17
extern test_t testExpr3; //((λ_x.x+1)+(λ_y.y*y))(λ_x.x+x)2 == 21
extern test_t testExpr4;
extern test_t testExpr5;
extern test_t testExpr6;
extern test_t testExpr7;
extern std::vector<test_t> tests;