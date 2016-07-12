#include "costest_alg.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>

using namespace std;

struct costest_alg_visitor : boost::static_visitor<int> {

	int operator()(Scalar op) const {
		return 1;
	}

	int operator()(VectorView op) const {
		return 1;
	}

	int operator()(Vector<Fix<F>> op) const {
		return accumulate(op.elements.begin(), op.elements.end(), 0,
			[](int c, shared_ptr<Fix<F>> e) {return c + e->cost; });
	}

	int operator()(Addition<Fix<F>> op) const {
		return op.left().cost + op.right().cost + 1;
	}

	int operator()(Multiplication<Fix<F>> op) const {
		return op.left().cost + op.right().cost + 1;
	}

	int operator()(Apply<Fix<F>> op) const {
		return op.lambda().cost + op.input().cost + 1;
	}

	int operator()(Lambda<Fix<F>> op) const {
		return op.body().cost;
	}

	int operator()(Variable op) const {
		return 1;
	}

	int operator()(Map_<Fix<F>> op) const {
		return op.lambda().cost *
			boost::get<Size_t>(boost::get<Power_t<Fix<TF>>>(op.vector().type).right()).size + 
			op.vector().cost + 1;
	}

	int operator()(Reduce_<Fix<F>> op) const {
		return op.lambda().cost *
			boost::get<Size_t>(boost::get<Power_t<Fix<TF>>>(op.vector().type).right()).size +
			op.vector().cost + 1;
	}

	int operator()(Zip_<Fix<F>> op) const {
		return op.lambda().cost *
			boost::get<Size_t>(boost::get<Power_t<Fix<TF>>>(op.vector_1().type).right()).size +
			op.vector_1().cost + op.vector_2().cost + 1;
	}
};

Fix<F> costest_alg(F<Fix<F>> node) {
	node.cost = boost::apply_visitor(costest_alg_visitor(), node.operation);
	return Fx(node);
}
