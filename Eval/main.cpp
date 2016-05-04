#include "big_test.h"
#include "evaluator.h"
#include "functional_test.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

int main() {
	default_random_engine rand;
	uniform_real_distribution<> dist;
	vector<double> *vec = new vector<double>(big_test_size), *row = new vector<double>(big_test_size);
	generate(vec->begin(), vec->end(), [&]() {return dist(rand); });
	generate(row->begin(), row->end(), [&]() {return dist(rand); });
	cout << vec->front() << " " << row->front();
	map<string, vector<double>*> bigVectors{ {"vec", vec}, {"row", row} };

	vector<double> times;
	auto evaluator = get_evaluator(mat_vec_mul, 10000000);
	auto start = chrono::high_resolution_clock::now();
	auto result = evaluator(bigVectors);
	auto done = chrono::high_resolution_clock::now();
	times.push_back(chrono::duration_cast<chrono::milliseconds>(done - start).count() / 1000.0);

	evaluator = get_evaluator(mat_vec_mul, 1000000);
	start = chrono::high_resolution_clock::now();
	result = evaluator(bigVectors);
	done = chrono::high_resolution_clock::now();
	times.push_back(chrono::duration_cast<chrono::milliseconds>(done - start).count() / 1000.0);

	evaluator = get_evaluator(mat_vec_mul, 500000);
	start = chrono::high_resolution_clock::now();
	result = evaluator(bigVectors);
	done = chrono::high_resolution_clock::now();
	times.push_back(chrono::duration_cast<chrono::milliseconds>(done - start).count() / 1000.0);

	evaluator = get_evaluator(mat_vec_mul, 1);
	start = chrono::high_resolution_clock::now();
	result = evaluator(bigVectors);
	done = chrono::high_resolution_clock::now();
	times.push_back(chrono::duration_cast<chrono::milliseconds>(done - start).count() / 1000.0);

	evaluator = get_evaluator(mat_vec_mul, 1, false);
	start = chrono::high_resolution_clock::now();
	result = evaluator(bigVectors);
	done = chrono::high_resolution_clock::now();
	times.push_back(chrono::duration_cast<chrono::milliseconds>(done - start).count() / 1000.0);

	cout << "sequential: " << times[0] << endl
		<< "parallel map: " << times[1] << endl
		<< "parallel fold: " << times[2] << endl
		<< "parallel zip: " << times[3] << endl
		<< "everything parallel: " << times[4] << endl;
}