#pragma once

#include "expr.hpp"
#include <boost\variant\apply_visitor.hpp>
#include <functional>
#include <type_traits>

// A horrible hack for the purpose of computing
// cata's return type. AnyF<F> stands in for a F<T>
// when T is unknown.
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

template<typename Fun, typename Tag>
struct functor_visitor;

template<typename Fun, typename Fa>
typename
functor_visitor<Fun, typename Fa::tag>::result_type
fmap(Fun f, Fa fa) {
	return boost::apply_visitor(
		functor_visitor<Fun, typename Fa::tag>{f}, fa);
}

//fmap: ExtExprF
template<typename Fun, typename A>
struct functor_visitor<Fun, ExtExprF<A>>
	: boost::static_visitor <
	ExtExprF<typename std::result_of<Fun(A)>::type >>
{
	typedef typename std::result_of<Fun(A)>::type B;

	explicit functor_visitor(Fun f)
		: f_(f) {}

	ExtExprF<B> operator()(Const_ i) const {
		return Const<B>(i.value);
	}

	ExtExprF<B> operator()(Add_<A> e) const {
		return Add(f_(e.left()), f_(e.right()));
	}

	ExtExprF<B> operator()(Mul_<A> e) const {
		return Mul(f_(e.left()), f_(e.right()));
	}

	ExtExprF<B> operator()(App_<A> e) const {
		return App(f_(e.function()), f_(e.input()));
	}

	ExtExprF<B> operator()(Lambda_<A> e) const {
		return Lam(f_(e.body()));
	}

	ExtExprF<B> operator()(Variable_ e) const {
		return Var<B>();
	}
private:
	Fun f_;
};

struct alg_visitor : boost::static_visitor<int> {

	alg_visitor(const std::map<Variable_, int>& subst) : subst(subst) {}

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
		return subst.at(e);
	}

	const std::map<Variable_, int>& subst;
};

//Fix<ExprF1> alg(ExprF2<int> e)
//{
//	return boost::apply_visitor(alg_visitor{}, e);
//}

int alg(ExtExprF<int> e) {
	return boost::apply_visitor(alg_visitor(e.subst), e);
}