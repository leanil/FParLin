#pragma once

#include "expr.hpp"
#include "fmap.hpp"
#include <boost\variant\apply_visitor.hpp>
#include <functional>
#include <type_traits>

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

int alg(F<int>);

struct alg_visitor : boost::static_visitor<int> {

	int operator()(Const_ i) const {
		return i.value;
	}

	int operator()(Add_<int> e) const {
		return e.left() + e.right();
	}

	int operator()(Mul_<int> e) const {
		return e.left() * e.right();
	}

	int operator()(App_<int> e) const {
		return e.function();
	}

	int operator()(Lambda_<int> e) const {
		return e.body();
	}

	int operator()(Variable_ e) const {
		return cata(alg, *e.value);
	}
};

int alg(F<int> e) {
	return boost::apply_visitor(alg_visitor(), e);
}