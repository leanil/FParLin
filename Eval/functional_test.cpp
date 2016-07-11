#include "functional_test.h"
#include "type.h"

static Fix<TF> double_to_double = Arrow(Double(), Double());
static auto x = Var(Double(), 'x'), y = Var(Double(), 'y');

// (2+3)*4 = 20
test_t testExpr1{
	(2_scl + 3_scl) * 4_scl,
	{20} };

//(λ_x.λ_y.x+y)5(4*3) = 17
test_t testExpr2{
	App(
		App(
			Lam(double_to_double, x,
				Lam(Double(), y,
					x + y)),
			5_scl),
		4_scl * 3_scl),
	{17} };

//(λ_y.y*y)(λ_x.x+x)2 = 16
test_t testExpr3{
	App(
		Lam(Double(), y, y * y),
		App(
			Lam(Double(), x, x + x),
			2_scl)),
	{16} };

test_t testExpr4{
	Vec({ testExpr1.first, testExpr2.first, testExpr3.first }),
	{20,17,16}};

//map(x->x+5, testExpr4)
test_t testExpr5{
	Map(
		App(
			Lam(double_to_double, x,
				Lam(Double(), y,
					x + y)),
			5_scl),
		testExpr4.first),
	{25,22,21} };

//Mátrix-vektor szorzás
auto v = Var(Power(Double(), 3_size), 'v');
test_t testExpr6{
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
					VecView("v", 3)))),
		Vec({
			Vec({ 1_scl, 2_scl, 3_scl }),
			Vec({ 4_scl, 5_scl, 6_scl }) })),
	{50, 122} };

//3 * a + b
static auto a = VecView("a", 3);
static auto b = VecView("b", 3);
static auto p = Var(Double(), 'p');
static auto q = Var(Double(), 'q');
test_t testExpr7{
	Zip(
		Lam(Arrow(Double(), Double()), p,
			Lam(Double(), q,
				p + q)),
		Map(
			Lam(Double(), p, 3_scl * p),
			a),
		b),
	{7, 11, 15} };

std::vector<test_t> tests{ testExpr1, testExpr2, testExpr3, testExpr4, testExpr5, testExpr6, testExpr7 };