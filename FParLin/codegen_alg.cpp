#include "codegen_alg.h"
#include "expr.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <algorithm>

struct codegen_alg_visitor : boost::static_visitor<codegen_t> {

	codegen_alg_visitor(int threshold, bool restricted) :
		threshold{ threshold }, restricted{ restricted } {}

	codegen_t operator()(Scalar node) const {
		return{ to_string(node.value),false };
	}

	codegen_t operator()(VectorView node) const {
		return{ "*bigVectors.at(\"" + node.id + "\")", false };
	}

	codegen_t operator()(Vector<codegen_t> node) const {
		string tmp = "make_vector({";
		for (auto element : node.elements) {
			tmp += element->first + ',';
		}
		tmp.pop_back();
		tmp += "})";
		return{ tmp,
			any_of(node.elements.begin(), node.elements.end(), [](shared_ptr<codegen_t> p) {return p->second; }) };
	}

	codegen_t operator()(Addition<codegen_t> node) const {
		return{ '(' + node.left().first + ")+(" + node.right().first + ')',
			node.left().second || node.right().second };
	}

	codegen_t operator()(Multiplication<codegen_t> node) const {
		return{ '(' + node.left().first + ")*(" + node.right().first + ')',
			node.left().second || node.right().second };
	}

	codegen_t operator()(Apply<codegen_t> node) const {
		return{ node.lambda().first + '(' + node.input().first + ')',
			node.lambda().second || node.input().second };
	}

	codegen_t operator()(Lambda<codegen_t> node) const {
		return{ string("[&](const auto& ") + node.id + "){return " + node.body().first + ";}",
			node.body().second };
	}

	codegen_t operator()(Variable node) const {
		return{ string(1,node.id), false };
	}

	codegen_t operator()(Map_<codegen_t> node) const {
		bool tmp = node.lambda().second || node.vector().second;
		return{ (threshold && node.cost > threshold && (!tmp || !restricted) ? "Par" : "") +
			("Map(" + node.lambda().first + "," + node.vector().first + ")"),
			threshold && node.cost > threshold };
	}

	codegen_t operator()(Reduce_<codegen_t> node) const {
		bool tmp = node.lambda().second || node.vector().second;
		return{ (threshold && node.cost > threshold && (!tmp || !restricted) ? "Par" : "") +
			("Reduce(" + node.lambda().first + "," + node.vector().first + ")"),
			threshold && node.cost > threshold };
	}

	codegen_t operator()(Zip_<codegen_t> node) const {
		bool tmp = node.lambda().second || node.vector_1().second || node.vector_2().second;
		return{ (threshold && node.cost > threshold && (!tmp || !restricted) ? "Par" : "") +
			("Zip(" + node.lambda().first + "," + node.vector_1().first + "," + node.vector_2().first + ")"),
			threshold && node.cost > threshold };
	}

	int threshold;
	bool restricted;
};


codegen_t codegen_alg::operator()(F<codegen_t> node) {
	return boost::apply_visitor(codegen_alg_visitor(threshold, restricted), node);
}
