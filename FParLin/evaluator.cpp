#include "evaluator.h"
#include "codegen_alg.h"
#include "costest_alg.h"
#include "typecheck_alg.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <windows.h> 

using namespace std;

function<vector<double>(map<string, vector<double>*>)> get_evaluator(Fix<F> tree, int threshold) {
	typecheck_t checked = cata(typecheck_alg, tree);
	if (!checked.second.empty()) {
		cerr << "type mismatches were found:\n";
		for (string error : checked.second) {
			cerr << error << endl;
		}
		return function<vector<double>(map<string, vector<double>*>)>{};
	}
	checked.first = cata(costest_alg, checked.first);
	ofstream result("result.cpp");
	vector<string> headers{ "\"fparlin.h\"", "<iostream>" , "<map>", "<string>", "<vector>" };
	for (string header : headers) {
		result << "#include " << header << endl;
	}
	result << "\nextern \"C\" {\n\t__declspec(dllexport) std::vector<double> evaluator(std::map<std::string, std::vector<double>*> bigVectors) {\n";
	result << "\t\treturn vectorize(" << cata(codegen_alg(threshold), checked.first).first << ");\n";
	result << "\t}\n}\n";
	result.close();

	ifstream config("config.txt");
	string build_tool;
	getline(config, build_tool);
	system((build_tool + " /LD /MDd -fmsc-version=1900 -Wno-return-type-c-linkage result.cpp").c_str());
	HINSTANCE dll = LoadLibrary("result.dll");
	if (!dll) {
		cerr << "could not load dll\n";
		return function<vector<double>(map<string, vector<double>*>)>{};
	}
	auto evaluator = GetProcAddress(dll, "evaluator");
	if (!evaluator) {
		cerr << "could not get function address\n";
		return function<vector<double>(map<string, vector<double>*>)>{};
	}
	return ((vector<double>(*)(map<string, vector<double>*>))evaluator);
}