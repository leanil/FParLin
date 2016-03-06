#pragma once

#include "expr.hpp"
#include <boost\variant\variant.hpp>

template<typename Fun, typename Tag>
struct functor_visitor;

template<typename Fun, typename Fa>
typename
functor_visitor<Fun, typename Fa::tag>::result_type
fmap(Fun f, Fa fa) {
	return boost::apply_visitor(
		functor_visitor<Fun, typename Fa::tag>{f}, fa);
}

template<typename Fun, typename A>
struct functor_visitor<Fun, F<A>>
	: boost::static_visitor <
	F<typename std::result_of<Fun(A)>::type >>
{
	typedef typename std::result_of<Fun(A)>::type B;

	explicit functor_visitor(Fun f) : f_(f) {}

	F<B> operator()(Scalar_ i) const {
		return Scalar_{ i.value };
	}

	F<B> operator()(Add_<A> e) const {
		return Add_<B>(f_(e.left()), f_(e.right()), e.values, e.subst);
	}

	F<B> operator()(Mul_<A> e) const {
		return Mul_<B>(f_(e.left()), f_(e.right()), e.values, e.subst);
	}

	F<B> operator()(App_<A> e) const {
		return App_<B>(f_(e.function()), f_(e.input()), e.values, e.subst);
	}

	F<B> operator()(Lambda_<A> e) const {
		return Lambda_<B>(f_(e.body()), e.id, e.values, e.subst);
	}

	F<B> operator()(Variable_ e) const {
		return Variable_{ e.id, e.value };
	}
private:
	Fun f_;
};