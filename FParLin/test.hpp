#pragma once

#include "alg.hpp"
#include "coalg.hpp"
#include "expr.hpp"
#include "fix.hpp"

// (2+3)*4 == 20
Fix<F> testExpr1 =
Fx(Mul(
	Fx(Add(
		Fx(Scl(2)),
		Fx(Scl(3))
		)),
	Fx(Scl(4))));
auto test1 = []() {return cata(alg, ana(coalg, testExpr1)) == 20; };

//((λ_x.λ_y.x+y)5)(4*3)
Fix<F> testExpr2 =
Fx(App(
	Fx(App(
		Fx(Lam('x',
			Fx(Lam('y',
				Fx(Add(
					Fx(Var('x')),
					Fx(Var('y')))))))),
		Fx(Scl(5)))),
	Fx(Mul(
		Fx(Scl(4)),
		Fx(Scl(3))))));
auto test2 = []() {return cata(alg, ana(coalg, testExpr2)) == 17; };

//((λ_x.x+1)+(λ_y.y*y))(λ_x.x+x)2
Fix<F> testExpr3 =
Fx(App(
	Fx(Add(
		Fx(Lam('x',
			Fx(Add(
				Fx(Var('x')),
				Fx(Scl(1)))))),
		Fx(Lam('y',
			Fx(Mul(
				Fx(Var('y')),
				Fx(Var('y')))))))),
	Fx(App(
		Fx(Lam('x',
			Fx(Add(
				Fx(Var('x')),
				Fx(Var('x')))))),
		Fx(Scl(2))))));
auto test3 = []() {return cata(alg, ana(coalg, testExpr3)) == 21; };
