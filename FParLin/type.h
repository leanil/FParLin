#ifndef TYPE_H
#define TYPE_H

#include "fix.h"
#include <boost/variant/variant.hpp>
#include <memory>

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
	std::shared_ptr<A> left_;
	std::shared_ptr<A> right_;
};

template<typename A>
struct Power_t {
	Power_t(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A& left() const { return *left_; }
	A& right() const { return *right_; }
	std::shared_ptr<A> left_;
	std::shared_ptr<A> right_;
};

template<typename A>
struct Arrow_t {
	Arrow_t(A left, A right) : left_(new A(left)), right_(new A(right)) {}
	A& left() const { return *left_; }
	A& right() const { return *right_; }
	std::shared_ptr<A> left_;
	std::shared_ptr<A> right_;
};

template<typename A>
using TF = boost::variant<
	Invalid_t,
	Int_t,
	Double_t,
	Size_t,
	Product_t<A>,
	Power_t<A>,
	Arrow_t<A>>;

Fix<TF> Invalid();

Fix<TF> Int();

Fix<TF> Double();

Fix<TF> operator""_size(unsigned long long x);

Fix<TF> Size(unsigned x);

Fix<TF> Power(Fix<TF> a, Fix<TF> b);

Fix<TF> Arrow(Fix<TF> a, Fix<TF> b);

#include "type_fmap.h"

#endif