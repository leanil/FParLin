#include "evaluator.h"
#include "fparlin_exception.h"
#include "codegen_alg.h"
#include "costest_alg.h"
#include "typecheck_alg.h"
#include <cstdlib>
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <windows.h> 
#elif __linux__
#include <dlfcn.h>
#endif

using namespace std;

static int result_id = 0;
static string result_name;

void process_tree(Fix<F> tree, int threshold, bool restricted) {
	typecheck_t checked = cata(typecheck_alg, tree);
	if (!checked.second.empty()) {
		throw type_mismatch_exception(
			to_string(checked.second.size()) + " type mismatches were found.",
			checked.second);
	}
	checked.first = cata(costest_alg, checked.first);
	string code = cata(codegen_alg(threshold, restricted), checked.first).first;
	ofstream result(result_name + ".cpp");
	vector<string> headers{ "\"fparlin.h\"", "<map>", "<string>", "<vector>" };
	string signature = "std::vector<double> evaluator(std::map<std::string, std::vector<double>*> bigVectors)";
	for (string header : headers) {
		result << "#include " << header << endl;
	}
	result << "\nextern \"C\" {\n\t"
#ifdef _WIN32
		<< "__declspec(dllexport) "
#endif
		<< signature << ";\n}\n\n"
		<< signature << " {\n\t\treturn vectorize(" << code << ");\n}\n";
	result.close();
}

void build_library() {
	ifstream config("config.txt");
	string build_tool, vcvars = "";
	getline(config, build_tool);
	string build_command;
	if (build_tool.find("cl.exe") != string::npos) {
		getline(config, vcvars);
		string common = "/LD /analyze- /GS- /W3 /fp:precise /Gd /Oy /EHsc /D \"_CONSOLE\" /D \"_LIB\" /D \"_UNICODE\" /D \"UNICODE\""
			" -fmsc-version=1900";
		//-Wno-return-type-c-linkage /wd4190
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
		build_command = "\"" + vcvars + build_tool + " " + common + " " + configuration + " " + platform + " " + result_name + ".cpp\"";
	}
	else if (build_tool.find("g++") != string::npos || build_tool.find("clang") != string::npos) {
		build_command = build_tool +
			" -shared -fPIC -std=c++14 -g -O3 -mavx -funsafe-math-optimizations -ffast-math " + result_name + ".cpp -pthread -o " + result_name + ".so";
	}
	//cout << "building with:\n" << build_command << endl;
	if (system((build_command + " > build.out 2>&1").c_str())) {
		throw compile_exception{ "Failed to build the evaluator. See build.out for more information."s };
	}
}

using evaluator_t = vector<double>(*)(map<string, vector<double>*>);

evaluator_t link_library() {
#ifdef _WIN32
	HINSTANCE dll = LoadLibrary((result_name + ".dll").c_str());
	if (!dll) {
		throw dll_exception{ "could not load library: " + to_string(GetLastError()) };
	}
	FARPROC evaluator = GetProcAddress(dll, "evaluator");
	if (!evaluator) {
		throw dll_exception{ "could not get function address: " + to_string(GetLastError()) };
	}
#elif __linux__
	void* dll = dlopen(("./" + result_name + ".so").c_str(), RTLD_NOW);
	if (!dll) {
		throw dll_exception{ "could not load library: "s + dlerror() };
	}
	dlerror();
	void* evaluator = dlsym(dll, "evaluator");
	auto error = dlerror();
	if (error) {
		throw dll_exception{ "could not get function address: "s + error };
	}
#endif
	return (evaluator_t)evaluator;
}

function<vector<double>(map<string, vector<double>*>)> get_evaluator(Fix<F> tree, int threshold, bool restricted) {
	result_name = "result_" + to_string(result_id);
	process_tree(tree, threshold, restricted);
	build_library();
	evaluator_t evaluator = link_library();
	++result_id;
	return evaluator;
}
