#include "codegen_alg.h"
#include "costest_alg.h"
#include "test.hpp"
#include "typecheck_alg.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void build(Fix<F> expr, int threshold) {
	typecheck_t checked = cata(typecheck_alg, expr);
	if (checked.second.empty()) {
		checked.first = cata(costest_alg, checked.first);
		cout << "cost: " << get_cost(checked.first) << endl;
		ofstream out("../Eval/result.cpp");
		vector<string> headers{ "\"config.hpp\"", "\"fparlin.hpp\"", "<iostream>" };
		for (string header : headers) {
			out << "#include " << header << endl;
		}
		out << "\nint main() {\n";
		out << "    std::cout << " << cata(codegen_alg(threshold), checked.first).first << " << std::endl;\n";
		out << "}";
	}
	else {
		for (string error : checked.second) {
			cout << error << endl;
		}
	}
}

int main() {
	build(testExpr6, 50);
}