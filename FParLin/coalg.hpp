#pragma once

#include "expr.hpp"
#include <boost\variant\apply_visitor.hpp>
#include <functional>
#include <type_traits>

struct coalg_visitor : boost::static_visitor<ExtExprF<Expr>>
{
	ExtExprF<Expr> operator()(Const_ i) const
	{
		return i.value;
	}

	int operator()(Add_<int> e) const
	{
		return e.left + e.right;
	}

	int operator()(Mul_<int> e) const
	{
		return e.left * e.right;
	}
};

template<typename T> struct TemplateGetter;

template< template<typename> class Te, typename OldType > struct TemplateGetter<Te<OldType>>
{
	using result = Fix<Te>;
};

template<typename CoAlg, typename A>
typename TemplateGetter< typename std::result_of<CoAlg(A)>::type >::result
ana(CoAlg coalg, A o)
{
	using std::placeholders::_1;
	return Fx(fmap(std::bind(&ana<CoAlg, A>, coalg, _1), coalg(o)));
}

ExprF<int> coalg(int s)
{
	if (s == 1) {
		return Const<int>(1);
	}
	return Add(1, s - 1);
}