#include "functional_test.h"
#include "type.h"

static Fix<TF> double_to_double = Arrow(Double(), Double());
static auto x = Var(Double(), 'x'), y = Var(Double(), 'y');
static auto add = Lam(double_to_double, x, Lam(Double(), y, x + y));

// (2+3)*4 == 20
Fix<F> testExpr1 =
(2_scl + 3_scl) * 4_scl;

//((λ_x.λ_y.x+y)5)(4*3) == 17
Fix<F> testExpr2 =
App(
	App(add, 5_scl),
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
			add,
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

Fix<F> x_vec = Vec({ x,x });
Fix<F> testExpr7 =
Fold(
	Lam(Arrow(Power(Power(Double(), 2_size), 3_size), Double()), x,
		Lam(Double(), Var(Power(Power(Double(), 2_size), 3_size), 'p'),
			x +
			Fold(
				Lam(Arrow(Power(Double(), 2_size), Double()), y,
					Lam(Double(), Var(Power(Double(), 2_size), 'q'),
						y +
						Fold(add, Var(Power(Double(), 2_size), 'q'), 0_scl))),
				Var(Power(Power(Double(), 2_size), 3_size), 'p'),
				0_scl))),
	Map(
		Lam(Power(Power(Double(), 2_size), 3_size), x,
			Vec({ x_vec, x_vec, x_vec })),
		Vec({ 1_scl, 2_scl, 3_scl, 4_scl })),
	0_scl);

Fix<F> testExpr8 =
Fold(
	add,
	Zip(
		Lam(double_to_double, x,
			Lam(Double(), y,
				x * y)),
		Vec({ 1_scl,2_scl,3_scl }),
		VecView("vec", 3)),
	0_scl);
//Vec({ Vec({1_scl,2_scl,3_scl}), Vec({ 1_scl,2_scl,3_scl }) });