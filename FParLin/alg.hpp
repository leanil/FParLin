#pragma once

#include "const.hpp"
#include "expr.hpp"
#include "fmap.hpp"
#include <boost/variant/apply_visitor.hpp>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <vector>

//AnyF<F> stands in for a F<T> when T is unknown.
template<template<typename> class F>
struct AnyF {
	template<typename T>
	operator F<T> &() const;
};

template<typename Alg, template<typename> class F>
typename std::result_of<Alg(AnyF<F>)>::type
cata(Alg alg, Fix<F> o) {
	using std::placeholders::_1;
	return alg(fmap(std::bind(&cata<Alg, F>, alg, _1),
		unFix(o)));
}

Const alg(F<Const>);

struct alg_visitor : boost::static_visitor<Const> {

	Const operator()(Scalar i) const {
		return i.value;
	}

	Const operator()(Vector<Const> e) const {
		vector<int> tmp(e.elements.size());
		transform(e.elements.begin(), e.elements.end(), tmp.begin(), [](shared_ptr<Const> c) {return (int)*c; });
		return tmp;
	}

	Const operator()(Addition<Const> e) const {
		return e.left() + e.right();
	}

	Const operator()(Multiplication<Const> e) const {
		return e.left() * e.right();
	}

	Const operator()(Apply<Const> e) const {
		return e.function();
	}

	Const operator()(Lambda<Const> e) const {
		return e.body();
	}

	Const operator()(Variable e) const {
		return cata(alg, *e.value);
	}
};

Const alg(F<Const> e) {
	return boost::apply_visitor(alg_visitor(), e);
}