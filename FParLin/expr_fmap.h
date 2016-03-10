#pragma once

#include "expr.h"
#include "falgebra.h"
#include <boost\variant\static_visitor.hpp>
#include <algorithm>
#include <iterator>
#include <memory>

template<typename Fun, typename A>
struct functor_visitor<Fun, F<A>>
: boost::static_visitor <
	F<typename std::result_of<Fun(A)>::type >>
{
	typedef typename std::result_of<Fun(A)>::type B;

	explicit functor_visitor(Fun f) : f(f) {}

	F<B> operator()(Scalar e) const {
		return Scalar{ e.value, e.type, e.cost };
	}

	F<B> operator()(VectorView e) const {
		return VectorView{ e.id, e.vector, e.size, e.type, e.cost };
	}

	F<B> operator()(Vector<A> e) const {
		vector<B> tmp;
		transform(e.elements.begin(), e.elements.end(), back_inserter(tmp), [&](shared_ptr<A> a) {return f(*a); });
		return Vector<B>(tmp, e.values, e.subst, e.type, e.cost);
	}

	F<B> operator()(Addition<A> e) const {
		return Addition<B>(f(e.left()), f(e.right()), e.values, e.subst, e.type, e.cost);
	}

	F<B> operator()(Multiplication<A> e) const {
		return Multiplication<B>(f(e.left()), f(e.right()), e.values, e.subst, e.type, e.cost);
	}

	F<B> operator()(Apply<A> e) const {
		return Apply<B>(f(e.lambda()), f(e.input()), e.values, e.subst, e.type, e.cost);
	}

	F<B> operator()(Lambda<A> e) const {
		return Lambda<B>(f(e.body()), e.id, e.values, e.subst, e.type, e.cost);
	}

	F<B> operator()(Variable e) const {
		return Variable{ e.type, e.id, e.value, e.cost };
	}

	F<B> operator()(Map_<A> e) const {
		return Map_<B>(f(e.lambda()), f(e.vector()), e.values, e.subst, e.type, e.cost);
	}

	F<B> operator()(Fold_<A> e) const {
		return Fold_<B>(f(e.lambda()), f(e.vector()), f(e.init()), e.values, e.subst, e.type, e.cost);
	}

	F<B> operator()(Zip_<A> e) const {
		return Zip_<B>(f(e.lambda()), f(e.vector_1()), f(e.vector_2()), e.values, e.subst, e.type, e.cost);
	}
private:
	Fun f;
};