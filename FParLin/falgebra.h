#pragma once

#include "fix.h"
#include <boost/variant/apply_visitor.hpp>
#include <functional>
#include <type_traits>

template<typename Fun, typename Tag>
struct functor_visitor;

template<typename Fun, typename Fa>
typename functor_visitor<Fun, typename Fa::tag>::result_type
fmap(Fun f, Fa fa) {
	return boost::apply_visitor(functor_visitor<Fun, typename Fa::tag>{f}, fa);
}

//AnyF<F> stands in for a F<T> when T is unknown.
template<template<typename> class F>
struct AnyF {
	template<typename T>
	operator F<T> &() const;
};

template<typename Alg, template<typename> class F>
std::result_of_t<Alg(AnyF<F>)> cata(Alg alg, Fix<F> o) {
	using std::placeholders::_1;
	return alg(fmap(std::bind(&cata<Alg, F>, alg, _1),unFix(o)));
}

template<typename T> struct TemplateGetter;

template< template<typename> class Te, typename OldType > struct TemplateGetter<Te<OldType>> {
	using result = Fix<Te>;
};

template<typename CoAlg, typename A>
typename TemplateGetter< typename std::result_of<CoAlg(A)>::type >::result
ana(CoAlg coalg, A o) {
	using std::placeholders::_1;
	return Fx(fmap(std::bind(&ana<CoAlg, A>, coalg, _1), coalg(o)));
}