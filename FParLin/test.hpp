#pragma once

#include "falgebra.h"
#include "codegen_alg.h"
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
auto test1 = []() {return cata(codegen_alg,  testExpr1); };

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
auto test2 = []() {return cata(codegen_alg, testExpr2); };

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
auto test3 = []() {return cata(codegen_alg, testExpr3); };

Fix<F> testExpr4 =
Fx(Vec({ testExpr1, testExpr2, testExpr1 }));
auto test4 = []() {return cata(codegen_alg, testExpr4); };

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
auto test5 = []() {return cata(codegen_alg, testExpr5); };

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
auto test6 = []() {return cata(codegen_alg, testExpr6); };

Fix<F> typeError1 =
Fx(Vec({
	Fx(Scl(0)),
	Fx(VecView("vec",1)) }));

Fix<F> typeError2 =
Fx(Add(
	Fx(Scl(0)),
	Fx(VecView("vec",1))));

Fix<F> typeError3 =
Fx(App(
	Fx(Scl(0)),
	Fx(Scl(0))));

Fix<F> typeError4 =
Fx(App(
	Fx(Lam(x,Fx(Double()),
		Fx(Var(Fx(Double()),'x')))),
	Fx(VecView("vec", 1))));

Fix<F> typeError5 =
Fx(Lam(x, double_to_double,
	Fx(Scl(0))));

Fix<F> typeError6 =
Fx(Map(
	Fx(Scl(0)),
	Fx(Scl(0))));

Fix<F> typeError7 =
Fx(Map(
	Fx(Lam(Var(Fx(Power(Fx(Double()),Fx(Value(1)))),'x'),Fx(Double()),
		Fx(Scl(0)))),
	Fx(VecView("vec", 1))));

Fix<F> typeError8 =
Fx(Fold(
	Fx(Lam(x, double_to_double,
		Fx(Scl(0)))),
	Fx(Scl(0)),
	Fx(Scl(0))));

Fix<F> typeError9 =
Fx(Fold(
	Fx(Lam(Var(Fx(Power(Fx(Double()), Fx(Value(1)))), 'x'), double_to_double,
		Fx(Lam(y, Fx(Double()),
			Fx(Scl(0)))))),
	Fx(Vec({ Fx(Var(Fx(Int()), 'x')) })),
	Fx(Scl(0))));

Fix<F> typeError10 =
Fx(Zip(
	Fx(Lam(Var(Fx(Power(Fx(Double()), Fx(Value(1)))), 'x'), double_to_double,
		Fx(Lam(y, Fx(Double()),
			Fx(Scl(0)))))),
	Fx(Vec({ Fx(Scl(0)), Fx(Scl(0)) })),
	Fx(Vec({ Fx(Var(Fx(Int()), 'x')) }))));
	

Fix<F> typeErrors =
Fx(Vec({ typeError1, typeError2, typeError3, typeError4, typeError5, typeError6, typeError7, typeError8, typeError9, typeError10 }));
