#pragma once

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <iostream>
#include <vector>

using namespace std;

using Const_ = boost::variant<double, vector<double>>;

struct Const : Const_ {
	Const() : Const_{ 0 } {}
	Const(double a) : Const_(a) {}
	Const(vector<double> a) : Const_(a) {}

	explicit operator double() {
		return boost::get<double>(*this);
	}

	explicit operator vector<double>() {
		return boost::get<vector<double>>(*this);
	}
};

Const operator+(const Const& a, const Const& b) {
	return boost::get<double>(a) + boost::get<double>(b);
}

Const operator*(const Const& a, const Const& b) {
	return boost::get<double>(a) * boost::get<double>(b);
}

struct const_printer : boost::static_visitor<void> {
	const_printer(ostream& out) : out{ out } {}

	void operator()(double i) const {
		out << i;
	}

	void operator()(const vector<double>& v) const {
		out << "[";
		for (unsigned i = 0; i < v.size(); ++i) {
			out << (i ? " " : "") << v[i];
		}
		out << "]";
	}

	ostream& out;
};

ostream& operator<<(ostream& out, const Const& c) {
	boost::apply_visitor(const_printer(out), c);
	return out;
}