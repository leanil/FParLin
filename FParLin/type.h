#ifndef TYPE_H
#define TYPE_H

#include "fix.h"
#include <type_traits>
template<typename T, typename U> using is_assignable = std::is_assignable<T, U>;
#include <boost/variant/variant.hpp>
#include <memory>

using namespace std;

struct Invalid_t {};

struct Int_t {};

struct Double_t {};

struct Size_t {
	unsigned size;
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
	Size_t,
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
	TF(Size_t a) : TypeF<A>(a) {}
	TF(Product_t<A> a) : TypeF<A>(a) {}
	TF(Power_t<A> a) : TypeF<A>(a) {}
	TF(Arrow_t<A> a) : TypeF<A>(a) {}
};

Fix<TF> Invalid();

Fix<TF> Int();

Fix<TF> Double();

Fix<TF> operator""_size(unsigned long long x);

Fix<TF> Size(unsigned x);

//template<typename A>
//Fix<TF> Product(A a, A b) { return Fx(TF<Fix<TF>>(Product_t<A>(a,b))); }

template<typename A>
Fix<TF> Power(A a, A b) { return Fx(TF<Fix<TF>>(Power_t<A>(a, b))); }

template<typename A>
Fix<TF> Arrow(A a, A b) { return Fx(TF<Fix<TF>>(Arrow_t<A>(a, b))); }

#include "type_fmap.h"

#endif