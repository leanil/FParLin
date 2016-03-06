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
				Fx(Lam(
					Fx(Mul(
						Fx(Var()),
						Fx(Var()))))),
				Fx(Const(6)))),
			Fx(Const(5))));
	cout << cata(alg, ana(coalg, testExpr2)) << endl;
}