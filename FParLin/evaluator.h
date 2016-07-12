#pragma once

#include "expr.h"
#include <fstream>
#include <functional>
#include <map>
#include <string>
#include <vector>

std::function<std::vector<double>(std::map<std::string, std::vector<double>*>)> 
get_evaluator(FF tree, int threshold, bool restricted = true);

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
	out << '{';
	for (unsigned i = 0; i < v.size(); ++i) {
		out << (i ? "," : "") << v[i];
	}
	out << '}';
	return out;
}
