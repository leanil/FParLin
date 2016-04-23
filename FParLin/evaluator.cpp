#include "evaluator.h"
#include "codegen_alg.h"
#include "costest_alg.h"
#include "typecheck_alg.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <windows.h> 

using namespace std;

bool process_tree(Fix<F> tree, int threshold) {
	typecheck_t checked = cata(typecheck_alg, tree);
	if (!checked.second.empty()) {
		cerr << "type mismatches were found:\n";
		for (string error : checked.second) {
			cerr << error << endl;
		}
		return false;
	}
	checked.first = cata(costest_alg, checked.first);
	ofstream result("result.cpp");
	vector<string> headers{ "\"fparlin.h\"", "<iostream>" , "<map>", "<string>", "<vector>" };
	string signature = "std::vector<double> evaluator(std::map<std::string, std::vector<double>*> bigVectors)";
	for (string header : headers) {
		result << "#include " << header << endl;
	}
	result << "\nextern \"C\" {\n\t__declspec(dllexport) " << signature << ";\n}\n\n"
		<< signature << " {\n\t\treturn vectorize(" << cata(codegen_alg(threshold), checked.first).first << ");\n}\n";
	result.close();
	return true;
}

bool build_dll() {
	ifstream config("config.txt");
	string build_tool, vcvars = "";
	getline(config, build_tool);
	if (build_tool.find("cl.exe") != string::npos) {
		getline(config, vcvars);
	}
	string common = "/LD /analyze- /GS- /W3 /fp:precise /Gd /Oy /EHsc /D \"_CONSOLE\" /D \"_LIB\" /D \"_UNICODE\" /D \"UNICODE\""
		" -fmsc-version=1900 /wd4190";
	//-Wno-return-type-c-linkage
#ifdef NDEBUG
	string configuration = "/GL /Gm- /Gy /Oi /Zi /Ox /MD /D \"NDEBUG\"";
#else
	string configuration = "/Gm /Gy- /Oi- /ZI /Od /RTC1 /MDd /D \"_DEBUG\"";
#endif
#if _WIN64 || __x86_64__
	string platform = "-m64";
	vcvars += vcvars.empty() ? "" : " x86_amd64";
#else
	string platform = "/D \"WIN32\" -m32";
	vcvars += vcvars.empty() ? "" : " x86";
#endif
	vcvars += vcvars.empty() ? "" : " && ";
	string build_command = "\"" + vcvars + build_tool + " " + common + " " + configuration + " " + platform + " result.cpp\"";
	cout << "building with:\n" << build_command << endl;
	return !system(build_command.c_str());
}

bool link_dll(FARPROC& evaluator) {
	HINSTANCE dll = LoadLibrary("result.dll");
	if (!dll) {
		cerr << "could not load dll\n";
		return false;
	}
	evaluator = GetProcAddress(dll, "evaluator");
	if (!evaluator) {
		cerr << "could not get function address\n";
		return false;
	}
	return true;
}

function<vector<double>(map<string, vector<double>*>)> get_evaluator(Fix<F> tree, int threshold) {
	FARPROC evaluator;
	if (!process_tree(tree, threshold) || !build_dll() || !link_dll(evaluator)) {
		return function<vector<double>(map<string, vector<double>*>)>{};
	}
	return ((vector<double>(*)(map<string, vector<double>*>))evaluator);
}