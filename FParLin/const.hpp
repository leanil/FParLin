#pragma once

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <iostream>
#include <vector>

using namespace std;

using Const_ = boost::variant<int, vector<int>>;

struct Const : Const_ {
	Const() : Const_{ 0 } {}
	Const(int a) : Const_(a) {}
	Const(vector<int> a) : Const_(a) {}

	explicit operator int() {
		return boost::get<int>(*this);
	}

	explicit operator vector<int>() {
		return boost::get<vector<int>>(*this);
	}
};

Const operator+(const Const& a, const Const& b) {
	return boost::get<int>(a) + boost::get<int>(b);
}

Const operator*(const Const& a, const Const& b) {
	return boost::get<int>(a) * boost::get<int>(b);
}

struct const_printer : boost::static_visitor<void> {
	const_printer(ostream& out) : out{ out } {}

	void operator()(int i) const {
		out << i;
	}

	void operator()(const vector<int>& v) const {
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