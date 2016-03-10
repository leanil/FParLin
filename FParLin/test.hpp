#pragma once

#include "falgebra.h"
#include "codegen_alg.hpp"
#include "expr.h"
#include "fix.h"
#include "type.h"
#include <vector>

// (2+3)*4 == 20
Fix<F> testExpr1 =
Fx(Mul(
	Fx(Add(
		Fx(Scl(2)),
		Fx(Scl(3))
		)),
	Fx(Scl(4))));
auto test1 = []() {return cata(alg,  testExpr1); };

Fix<TF> double_to_double = Fx(Arrow(Fx(Double()), Fx(Double())));
auto x = Var(Fx(Double()), 'x'), y = Var(Fx(Double()), 'y');

//((λ_x.λ_y.x+y)5)(4*3) == 17
Fix<F> testExpr2 =
Fx(App(
	Fx(App(
		Fx(Lam(x, double_to_double,
			Fx(Lam(
				y, Fx(Double()),
				Fx(Add(
					Fx(x),
					Fx(y))))))),
		Fx(Scl(5)))),
	Fx(Mul(
		Fx(Scl(4)),
		Fx(Scl(3))))));
auto test2 = []() {return cata(alg, testExpr2); };

//((λ_x.x+1)+(λ_y.y*y))(λ_x.x+x)2 == 21
Fix<F> testExpr3 =
Fx(App(
	Fx(Add(
		Fx(Lam(x, Fx(Double()),
			Fx(Add(
				Fx(x),
				Fx(Scl(1)))))),
		Fx(Lam(y, Fx(Double()),
			Fx(Mul(
				Fx(y),
				Fx(y))))))),
	Fx(App(
		Fx(Lam(x, Fx(Double()),
			Fx(Add(
				Fx(x),
				Fx(x))))),
		Fx(Scl(2))))));
auto test3 = []() {return cata(alg, testExpr3); };

Fix<F> testExpr4 =
Fx(Vec({ testExpr1, testExpr2, testExpr1 }));
auto test4 = []() {return cata(alg, testExpr4); };

Fix<F> testExpr5 =
Fx(Map(
	Fx(App(
		Fx(Lam(x, double_to_double,
			Fx(Lam(y, Fx(Double()),
				Fx(Add(
					Fx(x),
					Fx(y))))))),
		Fx(Scl(5)))),
	testExpr4));
auto test5 = []() {return cata(alg, testExpr5); };

auto v = Var(Fx(Power(Fx(Double()), Fx(Value(3)))), 'v');
Fix<F> testExpr6 =
Fx(Map(
	Fx(Lam(v, Fx(Double()),
		Fx(Fold(
			Fx(Lam(x, double_to_double,
				Fx(Lam(y, Fx(Double()),
					Fx(Add(
						Fx(x),
						Fx(y))))))),
			Fx(Zip(
				Fx(Lam(x, double_to_double,
					Fx(Lam(y, Fx(Double()),
						Fx(Mul(
							Fx(x),
							Fx(y))))))),
				Fx(v),
				Fx(VecView("vec",3)))),
			Fx(Scl(0)))))),
	Fx(Vec({
		Fx(Vec({ Fx(Scl(1)), Fx(Scl(2)), Fx(Scl(3)) })),
		Fx(Vec({ Fx(Scl(4)), Fx(Scl(5)), Fx(Scl(6)) })) }))));
auto test6 = []() {return cata(alg, testExpr6); };
