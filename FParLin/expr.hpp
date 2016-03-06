#pragma once

#include "fix.hpp"
#include <boost\variant\variant.hpp>
#include <map>
#include <memory>
#include <stack>

#include <iostream>

using namespace std;

struct Const_;
template<typename A> struct Add_;
template<typename A> struct Mul_;
template<typename A> struct App_;
template<typename A> struct Lambda_;
template<typename A> struct Variable_;

template<typename A>
using ExprF_ =
boost::variant<
	Const_,
	Add_<A>,
	Mul_<A>,
	App_<A>,
	Lambda_<A>,
	Variable_<A>
>;

template<typename A>
struct F : ExprF_<A> {
	typedef F<A> tag;
	F(Const_ c) : ExprF_<A>(c) {}
	F(Add_<A> c) : ExprF_<A>(c) {}
	F(Mul_<A> c) : ExprF_<A>(c) {}
	F(App_<A> c) : ExprF_<A>(c) {}
	F(Lambda_<A> c) : ExprF_<A>(c) {}
	F(Variable_<A> c) : ExprF_<A>(c) {}
};

struct Const_ {
	int value;
};

template<typename A>
struct Add_ {
	Add_(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	Add_(A left, A right, const std::stack<shared_ptr<Fix<F>>>& values, const std::map<char, shared_ptr<Fix<F>>>& subst):
		left_(new A(left)), right_(new A(right)), values{ values }, subst{ subst } {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
	std::stack<shared_ptr<Fix<F>>> values;
	std::map<char, shared_ptr<Fix<F>>> subst;
};

template<typename A>
struct Mul_ {
	Mul_(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	Mul_(A left, A right, const std::stack<shared_ptr<Fix<F>>>& values, const std::map<char, shared_ptr<Fix<F>>>& subst) :
		left_(new A(left)), right_(new A(right)), values{ values }, subst{ subst } {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
	std::stack<shared_ptr<Fix<F>>> values;
	std::map<char, shared_ptr<Fix<F>>> subst;
};

template<typename A>
struct App_ {
	App_(A function, A input) : function_(new A(function)), input_(new A(input)) {}
	App_(A function, A input, const std::stack<shared_ptr<Fix<F>>>& values, const std::map<char, shared_ptr<Fix<F>>>& subst) :
		function_(new A(function)), input_(new A(input)), values{ values }, subst{ subst } {}
	A& function() { return *function_; }
	A& input() { return *input_; }
	shared_ptr<A> function_;
	shared_ptr<A> input_;
	std::stack<shared_ptr<Fix<F>>> values;
	std::map<char, shared_ptr<Fix<F>>> subst;
};

template<typename A>
struct Lambda_ {
	Lambda_(A body) : body_(new A(body)) {}
	Lambda_(A body, const std::stack<shared_ptr<Fix<F>>>& values, const std::map<char, shared_ptr<Fix<F>>>& subst) :
		body_(new A(body)), values{ values }, subst{ subst } {}
	A& body() { return *body_; }
	shared_ptr<A> body_;
	std::stack<shared_ptr<Fix<F>>> values;
	std::map<char, shared_ptr<Fix<F>>> subst;
};

template<typename A>
struct Variable_ {
	char id;
	shared_ptr<Fix<F>> value;
};

// kényelmi függvények
template<typename A = Fix<F>>
F<A> Const(int val) { return Const_{ val }; }

template<typename A>
F<A> Add(A a, A b) { return Add_<A>{a, b}; }

template<typename A>
F<A> Mul(A a, A b) { return Mul_<A>{a, b}; }

template<typename A>
F<A> App(A a, A b) { return App_<A>{a, b}; }

template<typename A>
F<A> Lam(A a) { return Lambda_<A>{a}; }

template<typename A = Fix<F>>
F<A> Var() { return Variable_<A>{ 'x' }; }
