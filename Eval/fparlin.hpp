#pragma once

#include <ppl.h>
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <iostream>
#include <numeric>
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
auto ParMap(const F& f, const vector<E>& v) {
	vector<result_of_t<F(E)>> result(v.size());
	concurrency::parallel_transform(v.begin(), v.end(), result.begin(), f);
	return result;
}

template<typename F, typename E>
auto Map(const F& f, const vector<E>& v) {
	vector<result_of_t<F(E)>> result;
	transform(v.begin(), v.end(), back_inserter(result), f);
	return result;
}

template<typename F, typename E, typename S>
S Fold(const F& f, const vector<E>& v, const S& s) {
	return accumulate(v.begin(), v.end(), s, [&](const S& s, const E& e) {return f(s)(e); });
}

template<typename F, typename A, typename B>
auto Zip(const F& f, const vector<A>& a, const vector<B>& b) {
	vector<result_of_t<result_of_t<F(A)>(B)>> result;
	transform(a.begin(), a.end(), b.begin(), back_inserter(result), [&](const A& a, const B& b) {return f(a)(b); });
	return result;
}