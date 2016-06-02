#include "big_test.h"
#include "error_test.h"
#include "evaluator.h"
#include "fparlin_exception.h"
#include "functional_test.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

double measure_time(const function<vector<double>(map<string, vector<double>*>)>& evaluator, const map<string, vector<double>*>& bigVectors) {
	double min_time = 999999;
	for (int i = 0; i < 4; ++i) {
		auto start = chrono::high_resolution_clock::now();
		auto result = evaluator(bigVectors);
		auto done = chrono::high_resolution_clock::now();
		min_time = min(min_time, chrono::duration_cast<chrono::milliseconds>(done - start).count() / 1000.0);
	}
	return min_time;
}

void generate_random_vector(vector<double>& vec) {
	default_random_engine rand;
	uniform_real_distribution<> dist;
	generate(vec.begin(), vec.end(), [&]() {return dist(rand); });
}

void run_performance_test() {
	
	vector<double> *vec = new vector<double>(big_test_size), *row = new vector<double>(big_test_size);
	vector<double>* vec_to_sum = new vector<double>(100000000);
	generate_random_vector(*vec);
	generate_random_vector(*row);
	generate_random_vector(*vec_to_sum);
	map<string, vector<double>*> bigVectors{ { "vec", vec },{ "row", row },{ "vec_to_sum", vec_to_sum } };

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

void run_error_test() {
	get_evaluator(typeErrors, 1);
}

void run_functional_test() {
	map<string, vector<double>*> bigVectors{ { "vec", new vector<double>{7,8,9}} };
	cout << get_evaluator(testExpr6, 20)(bigVectors) << endl;
}

int main() {
	try {
		run_performance_test();
		//run_functional_test();
		//run_error_test();
	}
	catch (const type_mismatch_exception& e) {
		cout << e.what << endl;
		for (string error : e.errors) {
			cout << error << endl;
		}
	}
	catch (const FParLin_exception& e) {
		cout << e.what << endl;
	}
}
