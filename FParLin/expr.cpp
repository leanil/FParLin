#include "expr.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

Fix<F> operator""_scl(unsigned long long val) { return Fix<F>(F<Fix<F>>(Scalar{ (double)val })); }

Fix<F> operator""_scl(long double val) { return Fix<F>(F<Fix<F>>(Scalar{ (double)val })); }

Fix<F> VecView(string name, BigVector* vector) { return Fix<F>(F<Fix<F>>(VectorView{ name, vector, (unsigned)vector->size() })); }

Fix<F> VecView(string name, unsigned size) { return Fix<F>(F<Fix<F>>(VectorView{ name, nullptr, size })); }

struct type_getter : boost::static_visitor<Fix<TF>> {
	template<typename T>
	Fix<TF> operator()(T node) const { return node.type; }
};

struct cost_getter : boost::static_visitor<int> {
	template<typename T>
	int operator()(T node) const { return node.cost; }
};

Fix<TF> get_type(const Fix<F>& node) {
	return boost::apply_visitor(type_getter(), node);
}

int get_cost(const Fix<F>& node) {
	return boost::apply_visitor(cost_getter(), node);
}
