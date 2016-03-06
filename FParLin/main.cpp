#include "alg.hpp"
#include "coalg.hpp"
#include "expr.hpp"
#include "fix.hpp"
#include <iostream>
#include <memory>

using namespace std;

int main() {
	// (2+3)*4 == 20
	Fix<F> testExpr1 =
		Fx(Mul(
			Fx(Add(
				Fx(Const(2)),
				Fx(Const(3))
				)),
			Fx(Const(4))
			));
	cout << cata(alg, ana(coalg, testExpr1)) << endl;

	Fix<F> testExpr2 =
		Fx(Add(
			Fx(App(
				Fx(Lam('x',
					Fx(Mul(
						Fx(Var('x')),
						Fx(Var('x')))))),
				Fx(Const(6)))),
			Fx(Const(5))));
	cout << cata(alg, ana(coalg, testExpr2)) << endl;

	Fix<F> testExpr3 =
		Fx(App(
			Fx(App(
				Fx(Lam('x',
					Fx(Lam('y',
						Fx(Add(
							Fx(Var('x')),
							Fx(Var('y')))))))),
				Fx(Const(5)))),
			Fx(Mul(
				Fx(Const(4)),
				Fx(Const(3))))));
	cout << cata(alg, ana(coalg, testExpr3)) << endl;
}