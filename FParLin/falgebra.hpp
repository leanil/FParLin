#pragma once

#include "fix.hpp"
#include "fmap.hpp"
#include <functional>
#include <type_traits>

//AnyF<F> stands in for a F<T> when T is unknown.
template<template<typename> class F>
struct AnyF {
	template<typename T>
	operator F<T> &() const;
};

template<typename Alg, template<typename> class F>
result_of_t<Alg(AnyF<F>)> cata(Alg alg, Fix<F> o) {
	using std::placeholders::_1;
	return alg(fmap(std::bind(&cata<Alg, F>, alg, _1),
		unFix(o)));
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