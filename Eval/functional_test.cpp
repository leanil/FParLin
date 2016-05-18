#include "functional_test.h"
#include "type.h"

static Fix<TF> double_to_double = Arrow(Double(), Double());
static auto x = Var(Double(), 'x'), y = Var(Double(), 'y');

// (2+3)*4 == 20
Fix<F> testExpr1 =
(2_scl + 3_scl) * 4_scl;

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

//map(x->x+5, testExpr4)
Fix<F> testExpr5 =
Map(
	App(
		Lam(double_to_double, x,
			Lam(Double(), y,
				x + y)),
		5_scl),
	testExpr4);

//Mátrix-vektor szorzás
auto v = Var(Power(Double(), 3_size), 'v');
Fix<F> testExpr6 =
Map(
	Lam(Double(), v,
		Reduce(
			Lam(double_to_double, x,
				Lam(Double(), y,
					x + y)),
			Zip(
				Lam(double_to_double, x,
					Lam(Double(), y,
						x * y)),
				v,
				VecView("vec", 3)))),
	Vec({
		Vec({ 1_scl, 2_scl, 3_scl }),
		Vec({ 4_scl, 5_scl, 6_scl }) }));

//3 * a + b
static auto a = VecView("a", 10000000);
static auto b = VecView("b", 10000000);
static auto p = Var(Double(), 'p');
static auto q = Var(Double(), 'q');
Fix<F> testExpr7 =
Zip(
	Lam(Arrow(Double(), Double()), p,
		Lam(Double(), q,
			p + q)),
	Map(
		Lam(Double(), p, 3_scl * p),
		a),
	b);