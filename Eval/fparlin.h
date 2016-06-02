#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <iostream>
#include <numeric>
#include <thread>
#include <type_traits>
#include <vector>

using namespace std;

template<typename T>
vector<T> make_vector(initializer_list<T> list) {
	return vector<T>(list.begin(), list.end());
}

vector<double>& vectorize(vector<double>& v) {
	return v;
}

vector<double> vectorize(double d) {
	return vector<double>{d};
}

template<typename F, typename E>
auto Map(const F& f, const vector<E>& v) {
	vector<result_of_t<F(E)>> result(v.size());
	transform(v.begin(), v.end(), result.begin(), f);
	return result;
}

template<typename F, typename E>
E Reduce(const F& f, const vector<E>& v) {
	return accumulate(v.begin() + 1, v.end(), v.front(), [&](const E& s, const E& e) {return f(s)(e); });
}

template<typename F, typename A, typename B>
auto Zip(const F& f, const vector<A>& a, const vector<B>& b) {
	vector<result_of_t<result_of_t<F(A)>(B)>> result(a.size());
	transform(a.begin(), a.end(), b.begin(), result.begin(), [&](const A& a, const B& b) {return f(a)(b); });
	return result;
}

template<typename E>
unsigned batch_size(const vector<E>& v) {
	unsigned n = thread::hardware_concurrency();
	return (unsigned)v.size() / n + (v.size() % n != 0);
}

template<typename F, typename E>
auto ParMap(const F& f, const vector<E>& v) {
	vector<result_of_t<F(E)>> result(v.size());
	unsigned batch = batch_size(v);
	vector<thread> threads;
	threads.reserve(thread::hardware_concurrency());
	for (unsigned i = 0; i < v.size(); i += batch) {
		threads.push_back(thread([&,i]() {transform(v.begin() + i, i + batch < v.size() ? v.begin() + i + batch : v.end(), result.begin() + i, f); }));
	}
	for (thread& t : threads) {
		t.join();
	}
	return result;
}

template<typename F, typename E>
E ParReduce(const F& f, const vector<E>& v) {
	auto f2 = [&](const E& s, const E& e) {return f(s)(e); };
	unsigned batch = batch_size(v);
	vector<E> results(thread::hardware_concurrency());
	vector<thread> threads;
	threads.reserve(thread::hardware_concurrency());
	for (unsigned i = 0; i < v.size(); i += batch) {
		threads.push_back(thread([&,i]() {results[i/batch] =
			accumulate(v.begin() + i + 1, i + batch < v.size() ? v.begin() + i + batch : v.end(), *(v.begin() + i), f2); }));
	}
	for (thread& t : threads) {
		t.join();
	}
	return accumulate(results.begin() + 1, results.end(), results.front(), f2);
}

template<typename F, typename A, typename B>
auto ParZip(const F& f, const vector<A>& a, const vector<B>& b) {
	vector<result_of_t<result_of_t<F(A)>(B)>> result(a.size());
	unsigned batch = batch_size(a);
	vector<thread> threads;
	threads.reserve(thread::hardware_concurrency());
	for (unsigned i = 0; i < a.size(); i += batch) {
		threads.push_back(thread([&,i]() {transform(a.begin() + i, i + batch < a.size() ? a.begin() + i + batch : a.end(), b.begin() + i, result.begin() + i,
			[&](const A& a, const B& b) {return f(a)(b); }); }));
	}
	for (thread& t : threads) {
		t.join();
	}
	return result;
}