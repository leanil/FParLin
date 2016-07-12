#pragma once

#include "falgebra.h"
#include "type.h"
#include <boost/variant/static_visitor.hpp>
#include <type_traits>

template<typename Fun, typename A>
struct type_functor_visitor: boost::static_visitor<TF<std::result_of_t<Fun(A)>>>
{
	typedef std::result_of_t<Fun(A)> B;

	type_functor_visitor(Fun f) : f(f) {}

	TF<B> operator()(Invalid_t a) const {
		return a;
	}

	TF<B> operator()(Int_t a) const {
		return a;
	}

	TF<B> operator()(Double_t a) const {
		return a;
	}

	TF<B> operator()(Size_t a) const {
		return a;
	}

	TF<B> operator()(Product_t<A> a) const {
		return Product_t<B>(f(a.left()), f(a.right()));
	}

	TF<B> operator()(Power_t<A> a) const {
		return Power_t<B>(f(a.left()), f(a.right()));
	}

	TF<B> operator()(Arrow_t<A> a) const {
		return Arrow_t<B>(f(a.left()), f(a.right()));
	}

private:
	Fun f;
};

template<typename Fun, typename A>
typename type_functor_visitor<Fun, A>::result_type
fmap(Fun f, TF<A> fa) {
	return boost::apply_visitor(type_functor_visitor<Fun, A>(f), fa);
}