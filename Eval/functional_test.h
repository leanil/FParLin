#pragma once

#include "expr.h"

extern Fix<F> testExpr1; // (2+3)*4 == 20
extern Fix<F> testExpr2; //((λ_x.λ_y.x+y)5)(4*3) == 17
extern Fix<F> testExpr3; //((λ_x.x+1)+(λ_y.y*y))(λ_x.x+x)2 == 21
extern Fix<F> testExpr4;
extern Fix<F> testExpr5;
extern Fix<F> testExpr6;
extern Fix<F> testExpr7;