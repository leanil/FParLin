#include "expr.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

using namespace std;

FF operator""_scl(unsigned long long val) { return Fx(F<FF>{Scalar{ (double)val }}); }

FF operator""_scl(long double val) { return Fx(F<FF>{Scalar{ (double)val }}); }

FF VecView(string name, unsigned size) { return Fx<F>(F<FF>{VectorView{ name, size }}); }

FF Vec(std::initializer_list<FF> a) { return Fx(F<FF>{Vector<FF>(a)}); }

FF operator+(FF a, FF b) { return Fx(F<FF>{Addition<FF>(a, b)}); }

FF operator*(FF a, FF b) { return Fx(F<FF>{Multiplication<FF>(a, b)}); }

FF App(FF lambda, FF value) { return Fx(F<FF>{Apply<FF>(lambda, value)}); }

FF Lam(Fix<TF> return_type, F<FF> variable, FF body) {
	return Fx(F<FF>{Lambda<FF>(body, boost::get<Variable>(variable.operation).id), Arrow(variable.type, return_type)});
}

FF Var(Fix<TF> type, char id) { return Fx(F<FF>{Variable{ id }, type}); }

FF Map(FF lambda, FF vector) { return Fx(F<FF>{Map_<FF>(lambda, vector)}); }

FF Reduce(FF lambda, FF vector) { return Fx(F<FF>{Reduce_<FF>(lambda, vector)}); }

FF Zip(FF lambda, FF vector_1, FF vector_2) { return Fx(F<FF>{Zip_<FF>(lambda, vector_1, vector_2)}); }

