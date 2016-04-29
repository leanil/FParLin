#pragma once

#include "falgebra.h"
#include "type.h"
#include <boost/variant/static_visitor.hpp>
#include <type_traits>

template<typename Fun, typename A>
struct functor_visitor<Fun, TF<A>>: boost::static_visitor<TF<std::result_of_t<Fun(A)>>>
{
	typedef std::result_of_t<Fun(A)> B;

	explicit functor_visitor(Fun f) : f(f) {}

	TF<B> operator()(Invalid_t a) const {
		return a;
	}

	TF<B> operator()(Int_t a) const {
		return a;
	}

	TF<B> operator()(Double_t a) const {
		return a;
	}

	TF<B> operator()(Value_t a) const {
		return a;
	}

	TF<B> operator()(Product_t<A> a) const {
		return Product(f(a.left()), f(a.right()));
	}

	TF<B> operator()(Power_t<A> a) const {
		return Power(f(a.left()), f(a.right()));
	}

	TF<B> operator()(Arrow_t<A> a) const {
		return Arrow(f(a.left()), f(a.right()));
	}

private:
	Fun f;
};
