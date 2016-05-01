#include "expr.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

Fix<F> operator""_scl(unsigned long long val) { return Fix<F>(F<Fix<F>>(Scalar{ (double)val })); }

Fix<F> operator""_scl(long double val) { return Fix<F>(F<Fix<F>>(Scalar{ (double)val })); }

Fix<F> VecView(string name, BigVector* vector) { return Fix<F>(F<Fix<F>>(VectorView{ name, vector, (unsigned)vector->size() })); }

Fix<F> VecView(string name, unsigned size) { return Fix<F>(F<Fix<F>>(VectorView{ name, nullptr, size })); }

struct type_getter : boost::static_visitor<Fix<TF>> {
	Fix<TF> operator()(Scalar a) const { return a.type; }
	Fix<TF> operator()(VectorView a) const { return a.type; }
	Fix<TF> operator()(Vector<Fix<F>> a) const { return a.type; }
	Fix<TF> operator()(Addition<Fix<F>> a) const { return a.type; }
	Fix<TF> operator()(Multiplication<Fix<F>> a) const { return a.type; }
	Fix<TF> operator()(Apply<Fix<F>> a) const { return a.type; }
	Fix<TF> operator()(Lambda<Fix<F>> a) const { return a.type; }
	Fix<TF> operator()(Variable a) const { return a.type; }
	Fix<TF> operator()(Map_<Fix<F>> a) const { return a.type; }
	Fix<TF> operator()(Fold_<Fix<F>> a) const { return a.type; }
	Fix<TF> operator()(Zip_<Fix<F>> a) const { return a.type; }
};

struct cost_getter : boost::static_visitor<int> {
	int operator()(Scalar a) const { return a.cost; }
	int operator()(VectorView a) const { return a.cost; }
	int operator()(Vector<Fix<F>> a) const { return a.cost; }
	int operator()(Addition<Fix<F>> a) const { return a.cost; }
	int operator()(Multiplication<Fix<F>> a) const { return a.cost; }
	int operator()(Apply<Fix<F>> a) const { return a.cost; }
	int operator()(Lambda<Fix<F>> a) const { return a.cost; }
	int operator()(Variable a) const { return a.cost; }
	int operator()(Map_<Fix<F>> a) const { return a.cost; }
	int operator()(Fold_<Fix<F>> a) const { return a.cost; }
	int operator()(Zip_<Fix<F>> a) const { return a.cost; }
};

Fix<TF> get_type(const Fix<F>& a) {
	return boost::apply_visitor(type_getter(), a);
}

int get_cost(const Fix<F>& a) {
	return boost::apply_visitor(cost_getter(), a);
}
