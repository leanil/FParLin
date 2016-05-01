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

using namespace std;

template<typename A> struct F;

struct Scalar {
	double value;
	Fix<TF> type;
	int cost;
};

using BigVector = vector<double>;

struct VectorView {
	string id;
	BigVector* vector;
	unsigned size;
	Fix<TF> type;
	int cost;
};

template<typename A>
struct Vector {
	Vector(initializer_list<A> elem) {
		elements.resize(elem.size());
		transform(elem.begin(), elem.end(), elements.begin(), [](A a) {return make_shared<A>(a); });
	}

	Vector(vector<A> elem, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst, Fix<TF> type, int cost) :
		values{ values }, subst{ subst }, type{ type }, cost{ cost } {
		elements.resize(elem.size());
		transform(elem.begin(), elem.end(), elements.begin(), [](A a) {return make_shared<A>(a); });
	}

	vector<shared_ptr<A>> elements;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
	Fix<TF> type;
	int cost;
};

template<typename A>
struct Addition {
	Addition(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	Addition(A left, A right, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst, Fix<TF> type, int cost) :
		left_(new A(left)), right_(new A(right)), values{ values }, subst{ subst }, type{ type }, cost{ cost } {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
	Fix<TF> type;
	int cost;
};

template<typename A>
struct Multiplication {
	Multiplication(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	Multiplication(A left, A right, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst, Fix<TF> type, int cost) :
		left_(new A(left)), right_(new A(right)), values{ values }, subst{ subst }, type{ type }, cost{ cost } {}
	A& left() { return *left_; }
	A& right() { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
	Fix<TF> type;
	int cost;
};

template<typename A>
struct Apply {
	Apply(A lambda, A input) : lambda_(new A(lambda)), input_(new A(input)) {}
	Apply(A lambda, A input, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst, Fix<TF> type, int cost) :
		lambda_(new A(lambda)), input_(new A(input)), values{ values }, subst{ subst }, type{ type }, cost{ cost } {}
	A& lambda() { return *lambda_; }
	A& input() { return *input_; }
	shared_ptr<A> lambda_;
	shared_ptr<A> input_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
	Fix<TF> type;
	int cost;
};

template<typename A>
struct Lambda {
	Lambda(A body, char id, Fix<TF> type) : body_(new A(body)), id{ id }, type{ type } {}
	Lambda(A body, char id, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst, Fix<TF> type, int cost) :
		body_(new A(body)), id{ id }, values{ values }, subst{ subst }, type{ type }, cost{ cost } {}
	A& body() { return *body_; }
	shared_ptr<A> body_;
	char id;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
	Fix<TF> type;
	int cost;
};

struct Variable {
	Fix<TF> type;
	char id;
	shared_ptr<Fix<F>> value;
	int cost;
};

template<typename A>
struct Map_ {
	Map_(A lambda, A vector) : lambda_(new A(lambda)), vector_(new A(vector)) {}
	Map_(A lambda, A vector, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst, Fix<TF> type, int cost) :
		lambda_(new A(lambda)), vector_(new A(vector)), values{ values }, subst{ subst }, type{ type }, cost{ cost } {}
	A& lambda() { return *lambda_; }
	A& vector() { return *vector_; }
	shared_ptr<A> lambda_;
	shared_ptr<A> vector_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
	Fix<TF> type;
	int cost;
};

template<typename A>
struct Fold_ {
	Fold_(A lambda, A vector, A init) : lambda_(new A(lambda)), vector_(new A(vector)), init_(new A(init)) {}
	Fold_(A lambda, A vector, A init, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst, Fix<TF> type, int cost) :
		lambda_(new A(lambda)), vector_(new A(vector)), init_(new A(init)), values{ values }, subst{ subst }, type{ type }, cost{ cost } {}
	A& lambda() { return *lambda_; }
	A& vector() { return *vector_; }
	A& init() { return *init_; }
	shared_ptr<A> lambda_;
	shared_ptr<A> vector_;
	shared_ptr<A> init_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
	Fix<TF> type;
	int cost;
};

template<typename A>
struct Zip_ {
	Zip_(A lambda, A vector_1, A vector_2) : lambda_(new A(lambda)), vector_1_(new A(vector_1)), vector_2_(new A(vector_2)) {}
	Zip_(A lambda, A vector_1, A vector_2, const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst, Fix<TF> type, int cost) :
		lambda_(new A(lambda)), vector_1_(new A(vector_1)), vector_2_(new A(vector_2)), values{ values }, subst{ subst }, type{ type }, cost{ cost } {}
	A& lambda() { return *lambda_; }
	A& vector_1() { return *vector_1_; }
	A& vector_2() { return *vector_2_; }
	shared_ptr<A> lambda_;
	shared_ptr<A> vector_1_;
	shared_ptr<A> vector_2_;
	stack<shared_ptr<Fix<F>>> values;
	map<char, shared_ptr<Fix<F>>> subst;
	Fix<TF> type;
	int cost;
};

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
};

Fix<F> operator""_scl(unsigned long long val);

Fix<F> operator""_scl(long double val);

Fix<F> VecView(string name, BigVector* vector);

Fix<F> VecView(string name, unsigned size);

template<typename A>
Fix<F> Vec(initializer_list<A> a) { return Fx(F<Fix<F>>(Vector<A>(a))); }

template<typename A>
Fix<F> operator+(A a, A b) { return Fx(F<Fix<F>>(Addition<A>(a, b))); }

template<typename A>
Fix<F> operator*(A a, A b) { return Fx(F<Fix<F>>(Multiplication<A>(a, b))); }

template<typename A>
Fix<F> App(A a, A b) { return Fx(F<Fix<F>>(Apply<A>(a, b))); }

template<typename A>
Fix<F> Lam(Fix<TF> t, F<A> v, A a) { return Fx(F<Fix<F>>(Lambda<A>(a, boost::get<Variable>(v).id, Arrow(boost::get<Variable>(v).type, t)))); }

template<typename A = Fix<F>>
Fix<F> Var(Fix<TF> t, char id) { return Fx(F<Fix<F>>(Variable{ t, id })); }

template<typename A>
Fix<F> Map(A a, A b) { return Fx(F<Fix<F>>(Map_<A>(a, b))); }

template<typename A>
Fix<F> Fold(A a, A b, A c) { return Fx(F<Fix<F>>(Fold_<A>(a, b, c))); }

template<typename A>
Fix<F> Zip(A a, A b, A c) { return Fx(F<Fix<F>>(Zip_<A>(a, b, c))); }

Fix<TF> get_type(const Fix<F>& a);

int get_cost(const Fix<F>& a);

template<typename A>
bool is_of_type(Fix<F> a) {
	return get_type(a).type() == boost::typeindex::type_id<A>().type_info();
}

template<typename A>
bool is_of_type(Fix<TF> a) {
	return a.type() == boost::typeindex::type_id<A>().type_info();
}

#include "expr_fmap.h"

#endif
