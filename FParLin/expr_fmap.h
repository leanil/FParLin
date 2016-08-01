#pragma once

#include "expr.h"
#include "falgebra.h"
#include <boost/variant/static_visitor.hpp>
#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>

template<typename Fun, typename A>
struct functor_visitor: boost::static_visitor<ExprF<std::result_of_t<Fun(A)>>> {

	using B = std::result_of_t<Fun(A)>;

	functor_visitor(Fun f) : f(f) {}

	ExprF<B> operator()(Scalar op) const {
		return op;
	}

	ExprF<B> operator()(VectorView op) const {
		return op;
	}

	ExprF<B> operator()(const Vector<A>& op) const {
		vector<B> tmp(op.elements.size());
		transform(op.elements.begin(), op.elements.end(), tmp.begin(), [&] (shared_ptr<A> a) {return f(*a); });
		return Vector<B>(tmp);
	}

	ExprF<B> operator()(Addition<A> e) const {
		return Addition<B>(f(e.left()), f(e.right()));
	}

	ExprF<B> operator()(Multiplication<A> e) const {
		return Multiplication<B>(f(e.left()), f(e.right()));
	}

	ExprF<B> operator()(Apply<A> e) const {
		return Apply<B>(f(e.lambda()), f(e.input()));
	}

	ExprF<B> operator()(Lambda<A> e) const {
		return Lambda<B>(f(e.body()), e.id);
	}

	ExprF<B> operator()(Variable e) const {
		return Variable{e.id};
	}

	ExprF<B> operator()(Map_<A> e) const {
		return Map_<B>(f(e.lambda()), f(e.vector()));
	}

	ExprF<B> operator()(Reduce_<A> e) const {
		return Reduce_<B>(f(e.lambda()), f(e.vector()));
	}

	ExprF<B> operator()(Zip_<A> e) const {
		return Zip_<B>(f(e.lambda()), f(e.vector_1()), f(e.vector_2()));
	}

	Fun f;
};

template<typename Fun, typename A>
typename functor_visitor<Fun, A>::result_type
fmap(Fun f, ExprF<A> fa) {
	return boost::apply_visitor(functor_visitor<Fun, A>(f), fa);
}

template<typename Fun, typename A>
F<std::result_of_t<Fun(A)>>
fmap(Fun f, F<A> fa) {
	return F<std::result_of_t<Fun(A)>>{ fmap(f,fa.operation), fa.type, fa.cost };
}