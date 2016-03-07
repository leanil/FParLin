#pragma once

#include "fix.hpp"
#include <boost/variant/variant.hpp>
#include <algorithm>
#include <initializer_list>
#include <map>
#include <memory>
#include <stack>
#include <vector>

#include <iostream>

using namespace std;

struct Scalar;
template<typename A> struct Vector;
template<typename A> struct Addition;
template<typename A> struct Multiplication;
template<typename A> struct Apply;
template<typename A> struct Lambda;
struct Variable;
template<typename A> struct Map;

template<typename A>
using ExprF =
boost::variant<
	Scalar,
	Vector<A>,
	Addition<A>,
	Multiplication<A>,
	Apply<A>,
	Lambda<A>,
	Variable,
	Map<A>
>;

template<typename A>
struct F : ExprF<A> {
	typedef F<A> tag;
	F(Scalar c) : ExprF<A>(c) {}
	F(Vector<A> c) : ExprF<A>(c) {}
	F(Addition<A> c) : ExprF<A>(c) {}
	F(Multiplication<A> c) : ExprF<A>(c) {}
	F(Apply<A> c) : ExprF<A>(c) {}
	F(Lambda<A> c) : ExprF<A>(c) {}
	F(Variable c) : ExprF<A>(c) {}
	F(Map<A> c) : ExprF<A>(c) {}
};

struct Scalar {
	int value;
};

template<typename A>
struct Vector {
	Vector(initializer_list<A> elem,
		const stack<shared_ptr<Fix<F>>>& values = stack<shared_ptr<Fix<F>>>(),
		const map<char, shared_ptr<Fix<F>>>& subst = map<char, shared_ptr<Fix<F>>>()) :
		values{ values }, subst{ subst }
	{
		elements.resize(elem.size());
		transform(elem.begin(), elem.end(), elements.begin(), [](A a) {return make_shared<A>(a); });
	}

	Vector(vector<A> elem, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst) :
		values{ values }, subst{ subst }
	{
		elements.resize(elem.size());
		transform(elem.begin(), elem.end(), elements.begin(), [](A a) {return make_shared<A>(a); });
	}

	vector<shared_ptr<A>> elements;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
};

template<typename A>
struct Addition {
	Addition(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	Addition(A left, A right, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst) :
		left_(new A(left)), right_(new A(right)), values{ values }, subst{ subst } {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
};

template<typename A>
struct Multiplication {
	Multiplication(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	Multiplication(A left, A right, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst) :
		left_(new A(left)), right_(new A(right)), values{ values }, subst{ subst } {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
};

template<typename A>
struct Apply {
	Apply(A lambda, A input) : lambda_(new A(lambda)), input_(new A(input)) {}
	Apply(A lambda, A input, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst) :
		lambda_(new A(lambda)), input_(new A(input)), values{ values }, subst{ subst } {}
	A& lambda() { return *lambda_; }
	A& input() { return *input_; }
	shared_ptr<A> lambda_;
	shared_ptr<A> input_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
};

template<typename A>
struct Lambda {
	Lambda(A body, char id) : body_(new A(body)), id{ id } {}
	Lambda(A body, char id, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst) :
		body_(new A(body)), id{ id }, values{ values }, subst{ subst } {}
	A& body() { return *body_; }
	shared_ptr<A> body_;
	char id;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
};

struct Variable {
	char id;
	shared_ptr<Fix<F>> value;
};

template<typename A>
struct Map {
	Map(A lambda, A vector) : lambda_(new A(lambda)), vector_(new A(vector)) {}
	Map(A lambda, A vector, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst) :
		lambda_(new A(lambda)), vector_(new A(vector)), values{ values }, subst{ subst } {}
	A& lambda() { return *lambda_; }
	A& vector() { return *vector_; }
	shared_ptr<A> lambda_;
	shared_ptr<A> vector_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
};

// k�nyelmi f�ggv�nyek
template<typename A = Fix<F>>
F<A> Scl(int val) { return Scalar{ val }; }

template<typename A>
F<A> Vec(initializer_list<A> a) { return Vector<A>(a); }

template<typename A>
F<A> Add(A a, A b) { return Addition<A>(a, b); }

template<typename A>
F<A> Mul(A a, A b) { return Multiplication<A>(a, b); }

template<typename A>
F<A> App(A a, A b) { return Apply<A>(a, b); }

template<typename A>
F<A> Lam(char id, A a) { return Lambda<A>(a, id); }

template<typename A = Fix<F>>
F<A> Var(char id) { return Variable{ id }; }

template<typename A>
F<A> map(A a, A b) { return Map<A>(a, b); }