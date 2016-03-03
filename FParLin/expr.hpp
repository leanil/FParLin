#pragma once

#include "fix.hpp"
#include <boost\variant\recursive_wrapper.hpp>
#include <boost\variant\variant.hpp>
#include <map>
#include <memory>
#include <stack>

using namespace std;

struct Const_ {
	int value;
};

template<typename A>
struct Add_ {
	Add_(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A left() { return *left_; }
	A right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
};

template<typename A>
struct Mul_ {
	Mul_(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A left() { return *left_; }
	A right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
};

template<typename A>
struct App_ {
	App_(A function, A input) : function_(new A(function)), input_(new A(input)) {}
	A function() { return *function_; }
	A input() { return *input_; }
	shared_ptr<A> function_;
	shared_ptr<A> input_;
};

template<typename A>
struct Lambda_ {
	Lambda_(A body) : body_(new A(body)) {}
	A body() { return *body_; }
	shared_ptr<A> body_;
};

struct Variable_ {
	char id;
	bool operator<(const Variable_& v) const {
		return id < v.id;
	}
};

template<typename A>
using ExprF_ =
boost::variant<
	Const_,
	Add_<A>,
	Mul_<A>,
	App_<A>,
	Lambda_<A>,
	Variable_
>;

template<typename A>
struct ExprF : ExprF_<A> {
	typedef ExprF<A> tag;
	ExprF(Const_ c) : ExprF_<A>(c) {}
	ExprF(Add_<A> c) : ExprF_<A>(c) {}
	ExprF(Mul_<A> c) : ExprF_<A>(c) {}
	ExprF(App_<A> c) : ExprF_<A>(c) {}
	ExprF(Lambda_<A> c) : ExprF_<A>(c) {}
	ExprF(Variable_ c) : ExprF_<A>(c) {}
};

template<typename A>
struct ExtExprF : ExprF<A> {
	using ExprF<A>::ExprF;
	typedef ExtExprF<A> tag;
	std::stack<int> values;
	std::map<Variable_, int> subst;
};

using Expr = Fix<ExtExprF>;

// kényelmi függvények
template<typename A = Expr>
ExtExprF<A> Const(int val) { return Const_{ val }; }

template<typename A>
ExtExprF<A> Add(A a, A b) { return Add_<A>{a, b}; }

template<typename A>
ExtExprF<A> Mul(A a, A b) { return Mul_<A>{a, b}; }

template<typename A>
ExtExprF<A> App(A a, A b) { return App_<A>{a, b}; }

template<typename A>
ExtExprF<A> Lam(A a) { return Lambda_<A>{a}; }

template<typename A = Expr>
ExtExprF<A> Var() { 
	Variable_ var{ 'x' };
	ExtExprF<A> node(var);
	node.subst[var] = 6;
	return node;
}

