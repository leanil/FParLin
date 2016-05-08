#include "codegen_alg.h"
#include "expr.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <algorithm>

struct codegen_alg_visitor : boost::static_visitor<codegen_t> {

	codegen_alg_visitor(int threshold, bool restricted) :
		threshold{ threshold }, restricted{ restricted } {}

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
		return{ (threshold && e.cost > threshold && (!tmp || !restricted) ? "Par" : "") +
			("Map(" + e.lambda().first + "," + e.vector().first + ")"),
			threshold && e.cost > threshold };
	}

	codegen_t operator()(Reduce_<codegen_t> e) const {
		bool tmp = e.lambda().second || e.vector().second;
		return{ (threshold && e.cost > threshold && (!tmp || !restricted) ? "Par" : "") +
			("Reduce(" + e.lambda().first + "," + e.vector().first + ")"),
			threshold && e.cost > threshold };
	}

	codegen_t operator()(Zip_<codegen_t> e) const {
		bool tmp = e.lambda().second || e.vector_1().second || e.vector_2().second;
		return{ (threshold && e.cost > threshold && (!tmp || !restricted) ? "Par" : "") +
			("Zip(" + e.lambda().first + "," + e.vector_1().first + "," + e.vector_2().first + ")"),
			threshold && e.cost > threshold };
	}

	int threshold;
	bool restricted;
};


codegen_t codegen_alg::operator()(F<codegen_t> e) {
	return boost::apply_visitor(codegen_alg_visitor(threshold, restricted), e);
}
