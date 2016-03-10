#include "typecheck_alg.h"
#include "typeprinter_alg.h"
#include "test.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main() {
	//auto x = Var(Fx(Power(Fx(Double()), Fx(Value(3)))),'x');
	Fix<F> tree = cata(typecheck_alg, testExpr4);
	cout << cata(typeprinter_alg, get_type(tree)) << endl;

	ofstream out("../Eval/result.cpp");
	vector<string> headers{ "\"config.hpp\"", "\"fparlin.hpp\"", "<iostream>" };
	for (string header : headers) {
		out << "#include " << header << endl;
	}
	out << "\nint main() {\n";
	out << "    std::cout << " << test1() << " << std::endl;\n";
	out << "    std::cout << " << test2() << " << std::endl;\n";
	out << "    std::cout << " << test4() << " << std::endl;\n";
	out << "    std::cout << " << test5() << " << std::endl;\n";
	out << "    std::cout << " << test6() << " << std::endl;\n";
	out << "}";
}