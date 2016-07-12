#ifndef EXPR_H
#define EXPR_H

#include "fix.h"
#include "type.h"
#include <boost/type_index.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <algorithm>
#include <initializer_list>
#include <map>
#include <memory>
#include <stack>
#include <vector>

template<typename A> struct F;

struct Scalar {
	double value;
};

struct VectorView {
	std::string id;
	unsigned size;
};

template<typename A>
struct Vector {
	Vector(std::vector<A> elem) {
		elements.resize(elem.size());
		transform(elem.begin(), elem.end(), elements.begin(), [] (A a) {return std::make_shared<A>(a); });
	}
	std::vector<std::shared_ptr<A>> elements;
};

template<typename A>
struct Addition {
	Addition(A left, A right) :	left_(new A(left)), right_(new A(right)) {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	std::shared_ptr<A> left_;
	std::shared_ptr<A> right_;
};

template<typename A>
struct Multiplication {
	Multiplication(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	std::shared_ptr<A> left_;
	std::shared_ptr<A> right_;
};

template<typename A>
struct Apply {
	Apply(A lambda, A input) : lambda_(new A(lambda)), input_(new A(input)) {}
	A& lambda() { return *lambda_; }
	A& input() { return *input_; }
	std::shared_ptr<A> lambda_;
	std::shared_ptr<A> input_;
};

template<typename A>
struct Lambda {
	Lambda(A body, char id) : body_(new A(body)), id{ id } {}
	A& body() { return *body_; }
	std::shared_ptr<A> body_;
	char id;
};

struct Variable {
	char id;
};

template<typename A>
struct Map_ {
	Map_(A lambda, A vector) : lambda_(new A(lambda)), vector_(new A(vector)) {}
	A& lambda() { return *lambda_; }
	A& vector() { return *vector_; }
	std::shared_ptr<A> lambda_;
	std::shared_ptr<A> vector_;
};

template<typename A>
struct Reduce_ {
	Reduce_(A lambda, A vector) : lambda_(new A(lambda)), vector_(new A(vector)) {}
	A& lambda() { return *lambda_; }
	A& vector() { return *vector_; }
	std::shared_ptr<A> lambda_;
	std::shared_ptr<A> vector_;
};

template<typename A>
struct Zip_ {
	Zip_(A lambda, A vector_1, A vector_2) :
		lambda_(new A(lambda)), vector_1_(new A(vector_1)), vector_2_(new A(vector_2)) {}
	A& lambda() { return *lambda_; }
	A& vector_1() { return *vector_1_; }
	A& vector_2() { return *vector_2_; }
	std::shared_ptr<A> lambda_;
	std::shared_ptr<A> vector_1_;
	std::shared_ptr<A> vector_2_;
};

template<typename A>
using ExprF = boost::variant<
	Scalar,
	VectorView,
	Vector<A>,
	Addition<A>,
	Multiplication<A>,
	Apply<A>,
	Lambda<A>,
	Variable,
	Map_<A>,
	Reduce_<A>,
	Zip_<A>>;

template<typename A>
struct F {
	ExprF<A> operation;
	Fix<TF> type;
	int cost;
};

using FF = Fix<F>;

FF operator""_scl(unsigned long long val);

FF operator""_scl(long double val);

FF VecView(std::string name, unsigned size);

FF Vec(std::initializer_list<FF> a);

FF operator+(FF a, FF b);

FF operator*(FF a, FF b);

FF App(FF lambda, FF value);

FF Lam(Fix<TF> return_type, F<FF> variable, FF body);

FF Var(Fix<TF> type, char id);

FF Map(FF lambda, FF vector);

FF Reduce(FF lambda, FF vector);

FF Zip(FF lambda, FF vector_1, FF vector_2);

template<typename A>
bool is_of_type(FF a) {
	return a.type.type() == boost::typeindex::type_id<A>().type_info();
}

template<typename A>
bool is_of_type(Fix<TF> a) {
	return a.type() == boost::typeindex::type_id<A>().type_info();
}

#include "expr_fmap.h"

#endif
