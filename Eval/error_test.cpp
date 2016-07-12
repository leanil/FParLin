#include "error_test.h"
#include "type.h"

static Fix<TF> double_to_double = Arrow(Double(), Double());
static auto x = Var(Double(), 'x'), y = Var(Double(), 'y');

FF typeError1 =
Vec({ 0_scl,	VecView("vec",1) });

FF typeError2 =
0_scl + VecView("vec", 1);

FF typeError3 =
App(0_scl, 0_scl);

FF typeError4 =
App(
	Lam(Double(), x, Var(Double(), 'x')),
	VecView("vec", 1));

FF typeError5 =
Lam(double_to_double, x, 0_scl);

FF typeError6 =
Map(0_scl, 0_scl);

FF typeError7 =
Map(
	Lam(Double(), Var(Power(Double(), 1_size), 'x'), 0_scl),
	VecView("vec", 1));

FF typeError8 =
Reduce(
	Lam(double_to_double, x, 0_scl),
	0_scl);

FF typeError9 =
Reduce(
	Lam(double_to_double, Var(Power(Double(), 1_size), 'x'),
		Lam(Double(), y, 0_scl)),
	Vec({ Var(Int(), 'x') }));

FF typeError10 =
Zip(
	Lam(double_to_double, Var(Power(Double(), 1_size), 'x'),
		Lam(Double(), y, 0_scl)),
	Vec({ 0_scl, 0_scl }),
	Vec({ Var(Int(), 'x') }));


FF typeErrors =
Vec({ typeError1, typeError2, typeError3, typeError4, typeError5, typeError6, typeError7, typeError8, typeError9, typeError10 });
