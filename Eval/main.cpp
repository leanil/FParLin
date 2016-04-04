#include "evaluator.h"
#include "test.hpp"
#include <iostream>

using namespace std;

int main() {
	auto evaluator = get_evaluator(testExpr6, 50);
	map<string, vector<double>*> bigVectors{ {"vec", new vector<double>{7,8,9}} };
	auto result = evaluator(bigVectors);
	cout << result << endl;
}