#pragma once

#include "const.hpp"
#include "expr.hpp"
#include "falgebra.hpp"
#include <boost/variant/apply_visitor.hpp>
#include <algorithm>
#include <vector>

Const alg(F<Const>);

struct alg_visitor : boost::static_visitor<Const> {

	Const operator()(Scalar i) const {
		return i.value;
	}

	Const operator()(Vector<Const> e) const {
		vector<double> tmp(e.elements.size());
		transform(e.elements.begin(), e.elements.end(), tmp.begin(), [](shared_ptr<Const> c) {return (double)*c; });
		return tmp;
	}

	Const operator()(Addition<Const> e) const {
		return e.left() + e.right();
	}

	Const operator()(Multiplication<Const> e) const {
		return e.left() * e.right();
	}

	Const operator()(Apply<Const> e) const {
		return e.lambda();
	}

	Const operator()(Lambda<Const> e) const {
		return e.body();
	}

	Const operator()(Variable e) const {
		return cata(alg, *e.value);
	}

	Const operator()(Map_<Const> e) const {
		//TODO: Inplace evaluation not implemented.
		return;
	}
};

Const alg(F<Const> e) {
	return boost::apply_visitor(alg_visitor(), e);
}