#include "error_test.h"
#include "type.h"

static Fix<TF> double_to_double = Arrow(Double(), Double());
static auto x = Var(Double(), 'x'), y = Var(Double(), 'y');

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
