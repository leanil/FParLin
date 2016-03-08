#pragma once

#include "expr.hpp"
#include "fmap.hpp"
#include <boost/variant/apply_visitor.hpp>
#include <algorithm>
#include <functional>
#include <string>
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

string alg(F<string>);

struct alg_visitor : boost::static_visitor<string> {

	string operator()(Scalar i) const {
		return to_string(i.value);
	}

	string operator()(VectorView i) const {
		return "*bigVectors.at(\"" + i.id + "\")";
	}

	string operator()(Vector<string> e) const {
		string tmp = "make_vector({";
		for (auto element : e.elements) {
			tmp += *element + ',';
		}
		tmp.pop_back();
		tmp += "})";
		return tmp;
	}

	string operator()(Addition<string> e) const {
		return '(' + e.left() + ")+(" + e.right() + ')';
	}

	string operator()(Multiplication<string> e) const {
		return '(' + e.left() + ")*(" + e.right() + ')';
	}

	string operator()(Apply<string> e) const {
		return e.lambda() + '(' + e.input() + ')';
	}

	string operator()(Lambda<string> e) const {
		return string("[&](const auto& ") + e.id + "){return " + e.body() + ";}";
	}

	string operator()(Variable e) const {
		return string(1,e.id);
	}

	string operator()(Map_<string> e) const {
		return "Map(" + e.lambda() + "," + e.vector() + ")";
	}

	string operator()(Fold_<string> e) const {
		return "Fold(" + e.lambda() + "," + e.vector() + "," + e.init() + ")";
	}

	string operator()(Zip_<string> e) const {
		return "Zip(" + e.lambda() + "," + e.vector_1() + "," + e.vector_2() + ")";
	}
};

string alg(F<string> e) {
	return boost::apply_visitor(alg_visitor(), e);
}