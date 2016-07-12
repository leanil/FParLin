#include "codegen_alg.h"
#include "expr.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <algorithm>

using namespace std;

struct codegen_alg_visitor : boost::static_visitor<codegen_t> {

	codegen_alg_visitor(const F<codegen_t>& node, int threshold, bool restricted) :
		node{ node }, threshold{ threshold }, restricted{ restricted } {}

	codegen_t operator()(Scalar op) const {
		return{ to_string(op.value),false };
	}

	codegen_t operator()(VectorView op) const {
		return{ "*bigVectors.at(\"" + op.id + "\")", false };
	}

	codegen_t operator()(Vector<codegen_t> op) const {
		string tmp = "make_vector({";
		for (auto element : op.elements) {
			tmp += element->first + ',';
		}
		tmp.pop_back();
		tmp += "})";
		return{ tmp,
			any_of(op.elements.begin(), op.elements.end(), [](shared_ptr<codegen_t> p) {return p->second; }) };
	}

	codegen_t operator()(Addition<codegen_t> op) const {
		return{ '(' + op.left().first + ")+(" + op.right().first + ')',
			op.left().second || op.right().second };
	}

	codegen_t operator()(Multiplication<codegen_t> op) const {
		return{ '(' + op.left().first + ")*(" + op.right().first + ')',
			op.left().second || op.right().second };
	}

	codegen_t operator()(Apply<codegen_t> op) const {
		return{ op.lambda().first + '(' + op.input().first + ')',
			op.lambda().second || op.input().second };
	}

	codegen_t operator()(Lambda<codegen_t> op) const {
		return{ string("[&](const auto& ") + op.id + "){return " + op.body().first + ";}",
			op.body().second };
	}

	codegen_t operator()(Variable op) const {
		return{ string(1,op.id), false };
	}

	codegen_t operator()(Map_<codegen_t> op) const {
		bool tmp = op.lambda().second || op.vector().second;
		return{ (threshold && node.cost > threshold && (!tmp || !restricted) ? "Par" : "") +
			("Map(" + op.lambda().first + "," + op.vector().first + ")"),
			threshold && node.cost > threshold };
	}

	codegen_t operator()(Reduce_<codegen_t> op) const {
		bool tmp = op.lambda().second || op.vector().second;
		return{ (threshold && node.cost > threshold && (!tmp || !restricted) ? "Par" : "") +
			("Reduce(" + op.lambda().first + "," + op.vector().first + ")"),
			threshold && node.cost > threshold };
	}

	codegen_t operator()(Zip_<codegen_t> op) const {
		bool tmp = op.lambda().second || op.vector_1().second || op.vector_2().second;
		return{ (threshold && node.cost > threshold && (!tmp || !restricted) ? "Par" : "") +
			("Zip(" + op.lambda().first + "," + op.vector_1().first + "," + op.vector_2().first + ")"),
			threshold && node.cost > threshold };
	}

	const F<codegen_t>& node;
	int threshold;
	bool restricted;
};


codegen_t codegen_alg::operator()(F<codegen_t> node) {
	return boost::apply_visitor(codegen_alg_visitor(node, threshold, restricted), node.operation);
}
