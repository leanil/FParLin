#pragma once

#include "expr.hpp"
#include <boost\variant\variant.hpp>
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>

template<typename Fun, typename Tag>
struct functor_visitor;

template<typename Fun, typename Fa>
typename
functor_visitor<Fun, typename Fa::tag>::result_type
fmap(Fun f, Fa fa) {
	return boost::apply_visitor(functor_visitor<Fun, typename Fa::tag>{f}, fa);
}

template<typename Fun, typename A>
struct functor_visitor<Fun, F<A>>
	: boost::static_visitor <
	F<typename std::result_of<Fun(A)>::type >>
{
	typedef typename std::result_of<Fun(A)>::type B;

	explicit functor_visitor(Fun f) : f(f) {}

	F<B> operator()(Scalar e) const {
		return Scalar{ e.value };
	}

	F<B> operator()(Vector<A> e) const {
		vector<B> tmp;
		transform(e.elements.begin(), e.elements.end(), back_inserter(tmp), [&](shared_ptr<A> a) {return f(*a); });
		return Vector<B>(tmp, e.values, e.subst);
	}

	F<B> operator()(Addition<A> e) const {
		return Addition<B>(f(e.left()), f(e.right()), e.values, e.subst);
	}

	F<B> operator()(Multiplication<A> e) const {
		return Multiplication<B>(f(e.left()), f(e.right()), e.values, e.subst);
	}

	F<B> operator()(Apply<A> e) const {
		return Apply<B>(f(e.lambda()), f(e.input()), e.values, e.subst);
	}

	F<B> operator()(Lambda<A> e) const {
		return Lambda<B>(f(e.body()), e.id, e.values, e.subst);
	}

	F<B> operator()(Variable e) const {
		return Variable{ e.id, e.value };
	}

	F<B> operator()(Map<A> e) const {
		return Map<B>(f(e.lambda()), f(e.vector()), e.values, e.subst);
	}
private:
	Fun f;
};