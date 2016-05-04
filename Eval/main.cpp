#include "big_test.h"
#include "evaluator.h"
#include "functional_test.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

double measure_time(const function<vector<double>(map<string, vector<double>*>)>& evaluator, const map<string, vector<double>*>& bigVectors) {
	auto start = chrono::high_resolution_clock::now();
	auto result = evaluator(bigVectors);
	auto done = chrono::high_resolution_clock::now();
	return chrono::duration_cast<chrono::milliseconds>(done - start).count() / 1000.0;
}

int main() {
	default_random_engine rand;
	uniform_real_distribution<> dist;
	vector<double> *vec = new vector<double>(big_test_size), *row = new vector<double>(big_test_size);
	vector<double>* vec_to_sum = new vector<double>(100000000);
	generate(vec->begin(), vec->end(), [&]() {return dist(rand); });
	generate(row->begin(), row->end(), [&]() {return dist(rand); });
	generate(vec_to_sum->begin(), vec_to_sum->end(), [&]() {return dist(rand); });
	map<string, vector<double>*> bigVectors{ {"vec", vec}, {"row", row}, {"vec_to_sum", vec_to_sum} };

	vector<double> times;
	times.push_back(measure_time(get_evaluator(mat_vec_mul, 2000000000), bigVectors));
	times.push_back(measure_time(get_evaluator(mat_vec_mul, 100000000), bigVectors));
	times.push_back(measure_time(get_evaluator(mat_vec_mul, 50000000), bigVectors));
	times.push_back(measure_time(get_evaluator(mat_vec_mul, 1), bigVectors));
	times.push_back(measure_time(get_evaluator(mat_vec_mul, 1, false), bigVectors));

	times.push_back(measure_time(get_evaluator(vec_sum, 400000000), bigVectors));
	times.push_back(measure_time(get_evaluator(vec_sum, 1), bigVectors));

	cout << "matrix vector multiplication:\n"
		<< "sequential: " << times[0] << endl
		<< "parallel map: " << times[1] << endl
		<< "parallel fold: " << times[2] << endl
		<< "parallel zip: " << times[3] << endl
		<< "everything parallel: " << times[4] << endl
		<< "\nvector sum:\n"
		<< "sequential: " << times[5] << endl
		<< "parallel: " << times[6] << endl;
}
