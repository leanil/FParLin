#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <iostream>
#include <type_traits>
#include <vector>

using namespace std;

template<typename T>
vector<T> make_vector(initializer_list<T> list) {
	return vector<T>(list.begin(), list.end());
}

template<typename T>
ostream& operator<<(ostream& out, const vector<T>& v) {
	out << '{';
	for (unsigned i = 0; i < v.size(); ++i) {
		out << (i ? "," : "") << v[i];
	}
	out << '}';
	return out;
}

template<typename F, typename E>
vector<typename result_of<F(E)>::type> Map(const F& f, const vector<E>& v) {
	vector<typename result_of<F(E)>::type> result;
	transform(v.begin(), v.end(), back_inserter(result), f);
	return result;
}