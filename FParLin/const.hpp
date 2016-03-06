#pragma once

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <iostream>
#include <vector>

using namespace std;

using Const = boost::variant<int, vector<int>>;

Const operator+(const Const& a, const Const& b) {
	return boost::get<int>(a) + boost::get<int>(b);
}

Const operator*(const Const& a, const Const& b) {
	return boost::get<int>(a) * boost::get<int>(b);
}

bool operator==(const Const& c, int i) {
	return boost::get<int>(c) == i;
}

bool operator==(const Const& c, const vector<int>& v) {
	return boost::get<vector<int>>(c) == v;
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