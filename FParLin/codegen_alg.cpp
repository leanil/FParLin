#pragma once

#include "codegen_alg.h"
#include "expr.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <algorithm>

struct alg_visitor : boost::static_visitor<codegen_t> {

	alg_visitor(int threshold) : threshold{ threshold } {}

	codegen_t operator()(Scalar i) const {
		return{ to_string(i.value),false };
	}

	codegen_t operator()(VectorView i) const {
		return{ "*bigVectors.at(\"" + i.id + "\")", false };
	}

	codegen_t operator()(Vector<codegen_t> e) const {
		string tmp = "make_vector({";
		for (auto element : e.elements) {
			tmp += element->first + ',';
		}
		tmp.pop_back();
		tmp += "})";
		return{ tmp,
			any_of(e.elements.begin(), e.elements.end(), [](shared_ptr<codegen_t> p) {return p->second; }) };
	}

	codegen_t operator()(Addition<codegen_t> e) const {
		return{ '(' + e.left().first + ")+(" + e.right().first + ')',
			e.left().second || e.right().second };
	}

	codegen_t operator()(Multiplication<codegen_t> e) const {
		return{ '(' + e.left().first + ")*(" + e.right().first + ')',
			e.left().second || e.right().second };
	}

	codegen_t operator()(Apply<codegen_t> e) const {
		return{ e.lambda().first + '(' + e.input().first + ')',
			e.lambda().second || e.input().second };
	}

	codegen_t operator()(Lambda<codegen_t> e) const {
		return{ string("[&](const auto& ") + e.id + "){return " + e.body().first + ";}",
			e.body().second };
	}

	codegen_t operator()(Variable e) const {
		return{ string(1,e.id), false };
	}

	codegen_t operator()(Map_<codegen_t> e) const {
		bool tmp = e.lambda().second || e.vector().second;
		return{ (threshold && e.cost > threshold && !tmp ? "Par" : "") +
			("Map(" + e.lambda().first + "," + e.vector().first + ")"),
			threshold && e.cost > threshold };
	}

	codegen_t operator()(Fold_<codegen_t> e) const {
		bool tmp = e.lambda().second || e.vector().second || e.init().second;
		return{ (threshold && e.cost > threshold && !tmp ? "Par" : "") +
			("Fold(" + e.lambda().first + "," + e.vector().first + "," + e.init().first + ")"),
			threshold && e.cost > threshold };
	}

	codegen_t operator()(Zip_<codegen_t> e) const {
		bool tmp = e.lambda().second || e.vector_1().second || e.vector_2().second;
		return{ (threshold && e.cost > threshold && !tmp ? "Par" : "") +
			("Zip(" + e.lambda().first + "," + e.vector_1().first + "," + e.vector_2().first + ")"),
			threshold && e.cost > threshold };
	}

	int threshold;
};


codegen_t codegen_alg::operator()(F<codegen_t> e) {
	return boost::apply_visitor(alg_visitor(threshold), e);
}
