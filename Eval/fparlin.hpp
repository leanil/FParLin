#pragma once

#include <algorithm>
#include <cstdlib>
#include <initializer_list>
#include <iterator>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

class Preallocator {
public:
	Preallocator(initializer_list<pair<unsigned, unsigned>> sizes) {
		for (const auto& size : sizes) {
			for (unsigned i = 0; i < size.second; ++i) {
				memory[size.first].push_back({ malloc(size.first), true });
			}
		}
	}

	void reset() {
		for (auto& p : memory) {
			for_each(p.second.begin(), p.second.end(), [](pair<void*, bool>& mem) { mem.second = true; });
		}
	}

	~Preallocator() {
		for (auto& p : memory) {
			for_each(p.second.begin(), p.second.end(), [](pair<void*, bool>& mem) {free(mem.first); });
		}
	}

	static map<unsigned, list<pair<void*, bool>>> memory;
};

map<unsigned, list<pair<void*, bool>>> Preallocator::memory;

template <class T>
struct custom_allocator {
	typedef T value_type;
	custom_allocator() noexcept {}
	template <class U> custom_allocator(const custom_allocator<U>&) noexcept {}
	T* allocate(size_t n) { 
		unsigned size = (unsigned)n * sizeof(T);
		cout << "allocate: " << n << " " << size;
		if (Preallocator::memory.find(size) == Preallocator::memory.end() ||
			!Preallocator::memory[size].front().second) {
			cout << " no suitable preallocated memory found\n";
			return nullptr;
		}
		Preallocator::memory[size].splice(Preallocator::memory[size].end(), Preallocator::memory[size], Preallocator::memory[size].begin());
		Preallocator::memory[size].back().second = false;
		cout << " " << Preallocator::memory[size].back().first << endl;
		return static_cast<T*>(Preallocator::memory[size].back().first);
	}
	void deallocate(T* p, std::size_t n) {
		cout << "deallocate: " << n << " " << p << endl;
	}
};

template<typename T> struct Changer {
	using result = T;
};

template<typename T, template<typename> class A> struct Changer<std::vector<T, A<T>>> {
	using tmp = typename Changer<T>::result;
	using result = std::vector< tmp, custom_allocator<tmp> >;
};

template<typename T>
vector<T, custom_allocator<T>> make_vector(initializer_list<T> list) {
	cout << "make vec start\n";
	return vector<T, custom_allocator<T>>(list.begin(), list.end());
}

vector<double, custom_allocator<double>> vectorize(vector<double, custom_allocator<double>> v) {
	return v;
}

vector<double, custom_allocator<double>> vectorize(double d) {
	return vector<double, custom_allocator<double>>{d};
}

template<typename F, typename E, typename Alloc>
auto Map(const F& f, const vector<E, Alloc>& v) {
	cout << "map start\n";
	using elem_t = typename Changer<result_of_t<F(E)>>::result;
	vector<elem_t, custom_allocator<elem_t>> result(v.size());
	transform(v.begin(), v.end(), result.begin(), f);
	cout << "map end\n";
	return result;
}

template<typename F, typename E, typename S, typename Alloc>
S Fold(const F& f, const vector<E, Alloc>& v, const S& s) {
	cout << "fold start\n";
	return accumulate(v.begin(), v.end(), s, [&](const S& s, const E& e) {return f(s)(e); });
}

template<typename F, typename A, typename B, typename Alloc_1, typename Alloc_2>
auto Zip(const F& f, const vector<A, Alloc_1>& a, const vector<B, Alloc_2>& b) {
	cout << "zip start\n";
	using elem_t = typename Changer<result_of_t<result_of_t<F(A)>(B)>>::result;
	vector<elem_t, custom_allocator<elem_t>> result(a.size());
	transform(a.begin(), a.end(), b.begin(), result.begin(), [&](const A& a, const B& b) {return f(a)(b); });
	cout << "zip end\n";
	return result;
}

template<typename E, typename Alloc>
unsigned batch_size(const vector<E, Alloc>& v) {
	unsigned n = thread::hardware_concurrency();
	return v.size() / n + (v.size() % n != 0);
}

template<typename F, typename E, typename Alloc>
auto ParMap(const F& f, const vector<E, Alloc>& v) {
	using elem_t = typename Changer<result_of_t<F(E)>>::result;
	vector<elem_t, custom_allocator<elem_t>> result(v.size());
	unsigned batch = batch_size(v);
	vector<thread> threads;
	threads.reserve(thread::hardware_concurrency());
	for (unsigned i = 0; i < v.size(); i += batch) {
		threads.push_back(thread([&, i]() {transform(v.begin() + i, i + batch < v.size() ? v.begin() + i + batch : v.end(), result.begin() + i, f); }));
	}
	for (thread& t : threads) {
		t.join();
	}
	return result;
}

template<typename F, typename E, typename S, typename Alloc>
S ParFold(const F& f, const vector<E, Alloc>& v, const S& s) {
	auto f2 = [&](const S& s, const E& e) {return f(s)(e); };
	unsigned batch = batch_size(v);
	vector<S> results(thread::hardware_concurrency());
	vector<thread> threads;
	threads.reserve(thread::hardware_concurrency());
	for (unsigned i = 0; i < v.size(); i += batch) {
		threads.push_back(thread([&, i]() {results[i / batch] =
			accumulate(v.begin() + i + 1, i + batch < v.size() ? v.begin() + i + batch : v.end(), *(v.begin() + i), f2); }));
	}
	for (thread& t : threads) {
		t.join();
	}
	return accumulate(results.begin(), results.end(), s, f2);
}

template<typename F, typename A, typename B, typename Alloc_1, typename Alloc_2>
auto ParZip(const F& f, const vector<A, Alloc_1>& a, const vector<B, Alloc_2>& b) {
	using elem_t = typename Changer<result_of_t<result_of_t<F(A)>(B)>>::result;
	vector<elem_t, custom_allocator<elem_t>> result(a.size());
	unsigned batch = batch_size(a);
	vector<thread> threads;
	threads.reserve(thread::hardware_concurrency());
	for (unsigned i = 0; i < a.size(); i += batch) {
		threads.push_back(thread([&, i]() {transform(a.begin() + i, i + batch < a.size() ? a.begin() + i + batch : a.end(), b.begin() + i, result.begin() + i,
			[&](const A& a, const B& b) {return f(a)(b); }); }));
	}
	for (thread& t : threads) {
		t.join();
	}
	return result;
}