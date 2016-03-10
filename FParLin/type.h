#ifndef TYPE_H
#define TYPE_H

#include "fix.h"
#include <boost/variant/variant.hpp>
#include <memory>

using namespace std;

struct Invalid_t {};

struct Int_t {};

struct Double_t {};

struct Value_t {
	int value;
};

template<typename A>
struct Product_t {
	Product_t(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A& left() const { return *left_; }
	A& right() const { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
};

template<typename A>
struct Power_t {
	Power_t(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A& left() const { return *left_; }
	A& right() const { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
};

template<typename A>
struct Arrow_t {
	Arrow_t(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A& left() const { return *left_; }
	A& right() const { return *right_; }
	shared_ptr<A> left_;
	shared_ptr<A> right_;
};

template<typename A>
using TypeF = boost::variant<
	Invalid_t,
	Int_t,
	Double_t,
	Value_t,
	Product_t<A>,
	Power_t<A>,
	Arrow_t<A>
>;

template<typename A>
struct TF : TypeF<A> {
	typedef TF<A> tag;
	TF() : TypeF<A>(Invalid_t()) {}
	TF(Invalid_t a) : TypeF<A>(a) {}
	TF(Int_t a) : TypeF<A>(a) {}
	TF(Double_t a) : TypeF<A>(a) {}
	TF(Value_t a) : TypeF<A>(a) {}
	TF(Product_t<A> a) : TypeF<A>(a) {}
	TF(Power_t<A> a) : TypeF<A>(a) {}
	TF(Arrow_t<A> a) : TypeF<A>(a) {}
};

template<typename A = Fix<TF>>
TF<A> Invalid() { return Invalid_t(); }

template<typename A = Fix<TF>>
TF<A> Int() { return Int_t(); }

template<typename A = Fix<TF>>
TF<A> Double() { return Double_t(); }

template<typename A = Fix<TF>>
TF<A> Value(int x) { return Value_t{ x }; }

template<typename A>
TF<A> Product(A a, A b) { return Product_t<A>(a,b); }

template<typename A>
TF<A> Power(A a, A b) { return Power_t<A>(a, b); }

template<typename A>
TF<A> Arrow(A a, A b) { return Arrow_t<A>(a, b); }

#include "type_fmap.h"

#endif