#include "expr.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

Fix<F> operator""_scl(unsigned long long val) { return Fx(F<Fix<F>>{Scalar{ (double)val }}); }

Fix<F> operator""_scl(long double val) { return Fx(F<Fix<F>>{Scalar{ (double)val }}); }

Fix<F> VecView(string name, BigVector* vector) { return Fx(F<Fix<F>>{VectorView{ name, vector, (unsigned)vector->size() }}); }

Fix<F> VecView(string name, unsigned size) { return Fx<F>(F<Fix<F>>{VectorView{ name, nullptr, size }}); }
