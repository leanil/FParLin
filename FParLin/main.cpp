#include "test.hpp"
#include <fstream>

using namespace std;

int main() {
	ofstream out("../Eval/result.cpp");
	out << "#include \"fparlin.h\"\n#include <iostream>\n\nint main() {\n";
	out << "\tstd::cout << " << test1() << " << std::endl;\n";
	out << "\tstd::cout << " << test2() << " << std::endl;\n";
	out << "\tstd::cout << " << test4() << " << std::endl;\n";
	out << "}";
}