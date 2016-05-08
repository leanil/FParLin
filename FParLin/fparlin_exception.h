#pragma once

#include <list>
#include <string>

struct FParLin_exception {
	FParLin_exception(const std::string& what) : what{ what } {}
	std::string what;
};

struct type_mismatch_excetion : FParLin_exception {
	type_mismatch_excetion(const std::string& what, const std::list<std::string>& errors) :
		FParLin_exception{ what }, errors{ errors } {}
	std::list<std::string> errors;
};

struct compile_exception : FParLin_exception {
	using FParLin_exception::FParLin_exception;
};

struct dll_exception : FParLin_exception {
	using FParLin_exception::FParLin_exception;
};