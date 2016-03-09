#pragma once

#include "fix.hpp"
#include "type_expr.hpp"
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
struct VectorView;
template<typename A> struct Vector;
template<typename A> struct Addition;
template<typename A> struct Multiplication;
template<typename A> struct Apply;
template<typename A> struct Lambda;
struct Variable;
template<typename A> struct Map_;
template<typename A> struct Fold_;
template<typename A> struct Zip_;

template<typename A>
using ExprF =
boost::variant<
	Scalar,
	VectorView,
	Vector<A>,
	Addition<A>,
	Multiplication<A>,
	Apply<A>,
	Lambda<A>,
	Variable,
	Map_<A>,
	Fold_<A>,
	Zip_<A>
>;

template<typename A>
struct F : ExprF<A> {
	typedef F<A> tag;
	F(Scalar c) : ExprF<A>(c) {}
	F(VectorView c) : ExprF<A>(c) {}
	F(Vector<A> c) : ExprF<A>(c) {}
	F(Addition<A> c) : ExprF<A>(c) {}
	F(Multiplication<A> c) : ExprF<A>(c) {}
	F(Apply<A> c) : ExprF<A>(c) {}
	F(Lambda<A> c) : ExprF<A>(c) {}
	F(Variable c) : ExprF<A>(c) {}
	F(Map_<A> c) : ExprF<A>(c) {}
	F(Fold_<A> c) : ExprF<A>(c) {}
	F(Zip_<A> c) : ExprF<A>(c) {}

	Fix<TF> type;
	int cost;
};

struct Scalar {
	double value;
};

using BigVector = vector<double>;

struct VectorView {
	string id;
	BigVector* vector;
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
struct Map_ {
	Map_(A lambda, A vector) : lambda_(new A(lambda)), vector_(new A(vector)) {}
	Map_(A lambda, A vector, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst) :
		lambda_(new A(lambda)), vector_(new A(vector)), values{ values }, subst{ subst } {}
	A& lambda() { return *lambda_; }
	A& vector() { return *vector_; }
	shared_ptr<A> lambda_;
	shared_ptr<A> vector_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
};

template<typename A>
struct Fold_ {
	Fold_(A lambda, A vector, A init) : lambda_(new A(lambda)), vector_(new A(vector)), init_(new A(init)) {}
	Fold_(A lambda, A vector, A init, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst) :
		lambda_(new A(lambda)), vector_(new A(vector)), init_(new A(init)), values{ values }, subst{ subst } {}
	A& lambda() { return *lambda_; }
	A& vector() { return *vector_; }
	A& init() { return *init_; }
	shared_ptr<A> lambda_;
	shared_ptr<A> vector_;
	shared_ptr<A> init_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
};

template<typename A>
struct Zip_ {
	Zip_(A lambda, A vector_1, A vector_2) : lambda_(new A(lambda)), vector_1_(new A(vector_1)), vector_2_(new A(vector_2)) {}
	Zip_(A lambda, A vector_1, A vector_2, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst) :
		lambda_(new A(lambda)), vector_1_(new A(vector_1)), vector_2_(new A(vector_2)), values{ values }, subst{ subst } {}
	A& lambda() { return *lambda_; }
	A& vector_1() { return *vector_1_; }
	A& vector_2() { return *vector_2_; }
	shared_ptr<A> lambda_;
	shared_ptr<A> vector_1_;
	shared_ptr<A> vector_2_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
};

// kényelmi függvények
template<typename A = Fix<F>>
F<A> Scl(double val) { return Scalar{ val }; }

template<typename A = Fix<F>>
F<A> VecView(string name, BigVector* vector = nullptr) { return VectorView{ name, vector }; }

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
F<A> Map(A a, A b) { return Map_<A>(a, b); }

template<typename A>
F<A> Fold(A a, A b, A c) { return Fold_<A>(a, b, c); }

template<typename A>
F<A> Zip(A a, A b, A c) { return Zip_<A>(a, b, c); }