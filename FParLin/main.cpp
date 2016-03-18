#include "codegen_alg.h"
#include "test.hpp"
#include "typecheck_alg.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void build(Fix<F> expr) {
	typecheck_t checked = cata(typecheck_alg, expr);
	if (checked.second.empty()) {
		ofstream out("../Eval/result.cpp");
		vector<string> headers{ "\"config.hpp\"", "\"fparlin.hpp\"", "<iostream>" };
		for (string header : headers) {
			out << "#include " << header << endl;
		}
		out << "\nint main() {\n";
		out << "    std::cout << " << cata(codegen_alg, checked.first) << " << std::endl;\n";
		out << "}";
	}
	else {
		for (string error : checked.second) {
			cout << error << endl;
		}
	}
}

int main() {
	build(typeErrors);
}