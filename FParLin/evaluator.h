#pragma once

#include "expr.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

std::function<std::vector<double>(std::map<std::string, std::vector<double>*>)>  get_evaluator(Fix<F> tree, int threshold);

template<typename T>
ostream& operator<<(ostream& out, const vector<T>& v) {
	out << '{';
	for (unsigned i = 0; i < v.size(); ++i) {
		out << (i ? "," : "") << v[i];
	}
	out << '}';
	return out;
}