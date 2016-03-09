#include "costest_alg.h"

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <algorithm>
#include <vector>

struct alg_visitor : boost::static_visitor<Fix<F>> {

	Fix<F> operator()(Scalar a) const {
		F<Fix<F>> b = a;
		b.cost = 1;
		return Fx(b);
	}

	Fix<F> operator()(VectorView a) const {
		F<Fix<F>> b = a;
		b.cost = 1;
		return Fx(b);
	}

	Fix<F> operator()(Vector<Fix<F>> e) const {
		Fix<F> tmp = "make_vector({";
		for (auto element : e.elements) {
			tmp += *element + ',';
		}
		tmp.pop_back();
		tmp += "})";
		return tmp;
	}

	Fix<F> operator()(Addition<Fix<F>> e) const {
		return '(' + e.left() + ")+(" + e.right() + ')';
	}

	Fix<F> operator()(Multiplication<Fix<F>> e) const {
		return '(' + e.left() + ")*(" + e.right() + ')';
	}

	Fix<F> operator()(Apply<Fix<F>> e) const {
		return e.lambda() + '(' + e.input() + ')';
	}

	Fix<F> operator()(Lambda<Fix<F>> e) const {
		return Fix<F>("[&](const auto& ") + e.id + "){return " + e.body() + ";}";
	}

	Fix<F> operator()(Variable e) const {
		return Fix<F>(1, e.id);
	}

	Fix<F> operator()(Map_<Fix<F>> e) const {
		return "Map(" + e.lambda() + "," + e.vector() + ")";
	}

	Fix<F> operator()(Fold_<Fix<F>> e) const {
		return "Fold(" + e.lambda() + "," + e.vector() + "," + e.init() + ")";
	}

	Fix<F> operator()(Zip_<Fix<F>> e) const {
		return "Zip(" + e.lambda() + "," + e.vector_1() + "," + e.vector_2() + ")";
	}
};

Fix<F> costest_alg(F<Fix<F>> e) {
	return boost::apply_visitor(alg_visitor(), e);
}