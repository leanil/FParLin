#include "big_test.h"
#include "type.h"

static Fix<TF> double_to_double = Arrow(Double(), Double());
static auto x = Var(Double(), 'x'), y = Var(Double(), 'y');
static auto v = Var(Power(Double(), Size(big_test_size)), 'v');
static auto row = VecView("row", big_test_size);

// R^(16 x size) * R^size
FF mat_vec_mul =
Reduce(
	Lam(double_to_double, x,
		Lam(Double(), y,
			x + y)),
	Map(
		Lam(Double(), x, x + 1_scl),
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
						VecView("vec", big_test_size)))),
			Vec({row, row, row, row, row, row, row, row, row, row, row, row, row, row, row, row, }))));

FF vec_sum =
Reduce(
	Lam(double_to_double, x,
		Lam(Double(), y,
			x + y)),
	VecView("vec_to_sum", 100000000));