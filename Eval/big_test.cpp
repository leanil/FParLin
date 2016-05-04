#include "big_test.h"
#include "type.h"

static Fix<TF> double_to_double = Arrow(Double(), Double());
static auto x = Var(Double(), 'x'), y = Var(Double(), 'y');
static auto v = Var(Power(Double(), Size(big_test_size)), 'v');
static auto row = VecView("row", big_test_size);

// R^(16 x size) * R^size
Fix<F> mat_vec_mul =
Fold(
	Lam(double_to_double, x,
		Lam(Double(), y,
			x + y)),
	Map(
		Lam(Double(), x, x + 1_scl),
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
						VecView("vec", big_test_size)),
					0_scl)),
			Vec({row, row, row, row, row, row, row, row, row, row, row, row, row, row, row, row, }))),
	0_scl);

Fix<F> vec_sum =
Fold(
	Lam(double_to_double, x,
		Lam(Double(), y,
			x + y)),
	VecView("vec_to_sum", 100000000),
	0_scl
);