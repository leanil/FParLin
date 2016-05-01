#pragma once

#include "expr.h"
#include "fix.h"
#include "type.h"
#include <vector>

// (2+3)*4 == 20
Fix<F> testExpr1 =
(2_scl + 3_scl) * 4_scl;

Fix<TF> double_to_double = Arrow(Double(), Double());
auto x = Var(Double(), 'x'), y = Var(Double(), 'y');

//((λ_x.λ_y.x+y)5)(4*3) == 17
Fix<F> testExpr2 =
App(
	App(
		Lam(double_to_double, x,
			Lam(Double(), y,
				x + y)),
		5_scl),
	4_scl * 3_scl);

//((λ_x.x+1)+(λ_y.y*y))(λ_x.x+x)2 == 21
Fix<F> testExpr3 =
App(
	Lam(Double(), x, x + 1_scl) +
	Lam(Double(), y, y * y),
	App(
		Lam(Double(), x, x + x),
		2_scl));

Fix<F> testExpr4 =
Vec({ testExpr1, testExpr2, testExpr1 });

Fix<F> testExpr5 =
Map(
	App(
		Lam(double_to_double, x,
			Lam(Double(), y,
				x + y)),
		5_scl),
	testExpr4);

auto v = Var(Power(Double(), 3_size), 'v');
Fix<F> testExpr6 =
Map(
	Lam(Double(), v,
		Fold(
			Lam(double_to_double, x,
				Lam(Double(), y,
					x + y)),
			Zip(
				Lam(double_to_double, x,
					Lam(Double(), y,
						x * y)),
				v,
				VecView("vec", 3)),
			0_scl)),
	Vec({
		Vec({ 1_scl, 2_scl, 3_scl }),
		Vec({ 4_scl, 5_scl, 6_scl }) }));

Fix<F> typeError1 =
Vec({ 0_scl,	VecView("vec",1) });

Fix<F> typeError2 =
0_scl + VecView("vec", 1);

Fix<F> typeError3 =
App(0_scl, 0_scl);

Fix<F> typeError4 =
App(
	Lam(Double(), x, Var(Double(), 'x')),
	VecView("vec", 1));

Fix<F> typeError5 =
Lam(double_to_double, x, 0_scl);

Fix<F> typeError6 =
Map(0_scl, 0_scl);

Fix<F> typeError7 =
Map(
	Lam(Double(), Var(Power(Double(), 1_size), 'x'), 0_scl),
	VecView("vec", 1));

Fix<F> typeError8 =
Fold(
	Lam(double_to_double, x, 0_scl),
	0_scl,
	0_scl);

Fix<F> typeError9 =
Fold(
	Lam(double_to_double, Var(Power(Double(), 1_size), 'x'),
		Lam(Double(), y, 0_scl)),
	Vec({ Var(Int(), 'x') }),
	0_scl);

Fix<F> typeError10 =
Zip(
	Lam(double_to_double, Var(Power(Double(), 1_size), 'x'),
		Lam(Double(), y, 0_scl)),
	Vec({ 0_scl, 0_scl }),
	Vec({ Var(Int(), 'x') }));


Fix<F> typeErrors =
Vec({ typeError1, typeError2, typeError3, typeError4, typeError5, typeError6, typeError7, typeError8, typeError9, typeError10 });
