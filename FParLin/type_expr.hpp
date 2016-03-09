#pragma once

#include "fix.hpp"
#include <boost/variant/variant.hpp>
#include <memory>

using namespace std;

struct Int;
struct Double;
template<typename A> struct Multiplication;
template<typename A> struct Power;
template<typename A> struct Arrow;

template<typename A>
using TypeF = boost::variant<
	Int,
	Double,
	Multiplication<A>,
	Power<A>,
	Arrow<A>
>;

template<typename A>
struct TF : TypeF<A> {
	typedef TF<A> tag;
	TF(Int a) : TypeF<A>(a) {}
	TF(Double c) : TypeF<A>(c) {}
	TF(Multiplication<A> c) : TypeF<A>(c) {}
	TF(Power<A> c) : TypeF<A>(c) {}
	TF(Arrow<A> c) : TypeF<A>(c) {}
};

struct Int {};

struct Double {};

template<typename A>
struct Multiplication {
	Multiplication(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
};

template<typename A>
struct Power {
	Power(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
};

template<typename A>
struct Arrow {
	Arrow(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
};