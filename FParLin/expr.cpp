#include "expr.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

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

Fix<TF> get_type(const Fix<F>& a) {
	return boost::apply_visitor(type_getter(), a);
}