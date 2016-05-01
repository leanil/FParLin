#include "costest_alg.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>

struct costest_alg_visitor : boost::static_visitor<Fix<F>> {

	Fix<F> operator()(Scalar a) const {
		a.cost = 1;
		return Fx(F<Fix<F>>(a));
	}

	Fix<F> operator()(VectorView a) const {
		a.cost = 1;
		return Fx(F<Fix<F>>(a));
	}

	Fix<F> operator()(Vector<Fix<F>> a) const {
		a.cost = accumulate(a.elements.begin(), a.elements.end(), 0, [](int c, shared_ptr<Fix<F>> e) {return c + get_cost(*e); });
		return Fx(F<Fix<F>>(a));
	}

	Fix<F> operator()(Addition<Fix<F>> a) const {
		a.cost = get_cost(a.left()) + get_cost(a.right()) + 1;
		return Fx(F<Fix<F>>(a));
	}

	Fix<F> operator()(Multiplication<Fix<F>> a) const {
		a.cost = get_cost(a.left()) + get_cost(a.right()) + 1;
		return Fx(F<Fix<F>>(a));
	}

	Fix<F> operator()(Apply<Fix<F>> a) const {
		a.cost = get_cost(a.lambda()) + get_cost(a.input()) + 1;
		return Fx(F<Fix<F>>(a));
	}

	Fix<F> operator()(Lambda<Fix<F>> a) const {
		a.cost = get_cost(a.body());
		return Fx(F<Fix<F>>(a));
	}

	Fix<F> operator()(Variable a) const {
		a.cost = 1;
		return Fx(F<Fix<F>>(a));
	}

	Fix<F> operator()(Map_<Fix<F>> a) const {
		a.cost = get_cost(a.lambda())*
			boost::get<Size_t>(boost::get<Power_t<Fix<TF>>>(get_type(a.vector())).right()).size + 
			get_cost(a.vector()) + 1;
		return Fx(F<Fix<F>>(a));
	}

	Fix<F> operator()(Fold_<Fix<F>> a) const {
		a.cost = get_cost(a.lambda())*
			boost::get<Size_t>(boost::get<Power_t<Fix<TF>>>(get_type(a.vector())).right()).size +
			get_cost(a.vector()) + get_cost(a.init()) + 1;
		return Fx(F<Fix<F>>(a));
	}

	Fix<F> operator()(Zip_<Fix<F>> a) const {
		a.cost = get_cost(a.lambda())*
			boost::get<Size_t>(boost::get<Power_t<Fix<TF>>>(get_type(a.vector_1())).right()).size +
			get_cost(a.vector_1()) + get_cost(a.vector_2()) + 1;
		return Fx(F<Fix<F>>(a));
	}
};

Fix<F> costest_alg(F<Fix<F>> e) {
	return boost::apply_visitor(costest_alg_visitor(), e);
}
