#include "costest_alg.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>

struct costest_alg_visitor : boost::static_visitor<Fix<F>> {

	Fix<F> operator()(Scalar node) const {
		node.cost = 1;
		return Fx(F<Fix<F>>(node));
	}

	Fix<F> operator()(VectorView node) const {
		node.cost = 1;
		return Fx(F<Fix<F>>(node));
	}

	Fix<F> operator()(Vector<Fix<F>> node) const {
		node.cost = accumulate(node.elements.begin(), node.elements.end(), 0,
			[](int c, shared_ptr<Fix<F>> e) {return c + get_cost(*e); });
		return Fx(F<Fix<F>>(node));
	}

	Fix<F> operator()(Addition<Fix<F>> node) const {
		node.cost = get_cost(node.left()) + get_cost(node.right()) + 1;
		return Fx(F<Fix<F>>(node));
	}

	Fix<F> operator()(Multiplication<Fix<F>> node) const {
		node.cost = get_cost(node.left()) + get_cost(node.right()) + 1;
		return Fx(F<Fix<F>>(node));
	}

	Fix<F> operator()(Apply<Fix<F>> node) const {
		node.cost = get_cost(node.lambda()) + get_cost(node.input()) + 1;
		return Fx(F<Fix<F>>(node));
	}

	Fix<F> operator()(Lambda<Fix<F>> node) const {
		node.cost = get_cost(node.body());
		return Fx(F<Fix<F>>(node));
	}

	Fix<F> operator()(Variable node) const {
		node.cost = 1;
		return Fx(F<Fix<F>>(node));
	}

	Fix<F> operator()(Map_<Fix<F>> node) const {
		node.cost = get_cost(node.lambda())*
			boost::get<Size_t>(boost::get<Power_t<Fix<TF>>>(get_type(node.vector())).right()).size + 
			get_cost(node.vector()) + 1;
		return Fx(F<Fix<F>>(node));
	}

	Fix<F> operator()(Reduce_<Fix<F>> node) const {
		node.cost = get_cost(node.lambda())*
			boost::get<Size_t>(boost::get<Power_t<Fix<TF>>>(get_type(node.vector())).right()).size +
			get_cost(node.vector()) + 1;
		return Fx(F<Fix<F>>(node));
	}

	Fix<F> operator()(Zip_<Fix<F>> node) const {
		node.cost = get_cost(node.lambda())*
			boost::get<Size_t>(boost::get<Power_t<Fix<TF>>>(get_type(node.vector_1())).right()).size +
			get_cost(node.vector_1()) + get_cost(node.vector_2()) + 1;
		return Fx(F<Fix<F>>(node));
	}
};

Fix<F> costest_alg(F<Fix<F>> node) {
	return boost::apply_visitor(costest_alg_visitor(), node);
}
