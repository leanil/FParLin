#include "typecheck_alg.h"
#include "falgebra.h"
#include "type.h"
#include "typeprinter_alg.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/static_visitor.hpp>
#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>

using namespace std;

struct typecheck_result {
	ExprF<FF> operation;
	Fix<TF> type;
	list<string> errors;
};

struct typecheck_alg_visitor : boost::static_visitor<typecheck_result> {

	typecheck_alg_visitor(F<typecheck_t>& node) : node{ node } {}

	typecheck_result operator()(Scalar op) const {
		return{ op, Double(), list<string>() };
	}

	typecheck_result operator()(VectorView op) const {
		auto type = Power(Double(), Size(op.size));
		return{ op, type, list<string>() };
	}

	typecheck_result operator()(Vector<typecheck_t> op) const {
		list<string> errors;
		for (auto e : op.elements) {
			errors.insert(errors.end(), e->second.begin(), e->second.end());
		}
		vector<FF> elements;
		transform(op.elements.begin(), op.elements.end(), back_inserter(elements),
			[] (shared_ptr<typecheck_t> p) {return p->first; });
		Vector<FF> b(elements);
		if (errors.empty()) {
			string type_str = cata(typeprinter_alg, b.elements[0]->type);
			unsigned i;
			for (i = 1; i < b.elements.size(); ++i) {
				string elem_type = cata(typeprinter_alg, b.elements[i]->type);
				if (elem_type != type_str) {
					errors.push_back(type_str + " != " + elem_type);
					break;
				}
			}
		}
		auto type = errors.empty() ? Power(b.elements[0]->type, Size((unsigned)b.elements.size())) : Invalid();
		return{ b, type, errors };
	}

	typecheck_result operator()(Addition<typecheck_t> op) const {
		list<string> errors;
		errors.insert(errors.end(), op.left().second.begin(), op.left().second.end());
		errors.insert(errors.end(), op.right().second.begin(), op.right().second.end());
		Addition<FF> b(op.left().first, op.right().first);
		if (errors.empty()) {
			if (!is_of_type<Int_t>(b.left()) && !is_of_type<Double_t>(b.left())) {
				errors.push_back(cata(typeprinter_alg, b.left().type) + " not scalar");
			}
			if (!is_of_type<Int_t>(b.right()) && !is_of_type<Double_t>(b.right())) {
				errors.push_back(cata(typeprinter_alg, b.right().type) + " not scalar");

			}
		}
		auto type = errors.empty() ?
			(is_of_type<Int_t>(b.left()) && is_of_type<Int_t>(b.right()) ? Int() : Double()) :
			Invalid();
		return{ b, type, errors };
	}

	typecheck_result operator()(Multiplication<typecheck_t> op) const {
		list<string> errors;
		errors.insert(errors.end(), op.left().second.begin(), op.left().second.end());
		errors.insert(errors.end(), op.right().second.begin(), op.right().second.end());
		Multiplication<FF> b(op.left().first, op.right().first);
		if (errors.empty()) {
			if (!is_of_type<Int_t>(b.left()) && !is_of_type<Double_t>(b.left())) {
				errors.push_back(cata(typeprinter_alg, b.left().type) + " not scalar");
			}
			if (!is_of_type<Int_t>(b.right()) && !is_of_type<Double_t>(b.right())) {
				errors.push_back(cata(typeprinter_alg, b.right().type) + " not scalar");

			}
		}
		auto type = errors.empty() ?
			(is_of_type<Int_t>(b.left()) && is_of_type<Int_t>(b.right()) ? Int() : Double()) :
			Invalid();
		return{ b, type, errors };
	}

	typecheck_result operator()(Apply<typecheck_t> op) const {
		list<string> errors;
		errors.insert(errors.end(), op.lambda().second.begin(), op.lambda().second.end());
		errors.insert(errors.end(), op.input().second.begin(), op.input().second.end());
		Apply<FF> b(op.lambda().first, op.input().first);
		Fix<TF> type;
		if (errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda())) {
				errors.push_back(cata(typeprinter_alg, b.lambda().type) + " not a lambda");
			}
			else {
				auto lambda_type = boost::get<Arrow_t<Fix<TF>>>(b.lambda().type);
				string param_type = cata(typeprinter_alg, lambda_type.left());
				string input_type = cata(typeprinter_alg, b.input().type);
				if (param_type != input_type) {
					errors.push_back(param_type + " != " + input_type);
				}
				type = errors.empty() ? lambda_type.right() : Invalid();
			}
		}
		return{ b, type, errors };
	}

	typecheck_result operator()(Lambda<typecheck_t> op) const {
		list<string> errors(op.body().second);
		Lambda<FF> b(op.body().first, op.id);
		Fix<TF> type;
		if (errors.empty()) {
			auto lambda_type = boost::get<Arrow_t<Fix<TF>>>(node.type);
			string return_type = cata(typeprinter_alg, lambda_type.right());
			string body_type = cata(typeprinter_alg, b.body().type);
			if (return_type != body_type) {
				errors.push_back(return_type + " != " + body_type);
			}
			type = errors.empty() ? node.type : Invalid();
		}
		return{ b, type, errors };
	}

	typecheck_result operator()(Variable op) const {
		return{ op, node.type, list<string>() };
	}

	typecheck_result operator()(Map_<typecheck_t> op) const {
		list<string> errors;
		errors.insert(errors.end(), op.lambda().second.begin(), op.lambda().second.end());
		errors.insert(errors.end(), op.vector().second.begin(), op.vector().second.end());
		Map_<FF> b(op.lambda().first, op.vector().first);
		Fix<TF> type;
		if (errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda())) {
				errors.push_back(cata(typeprinter_alg, b.lambda().type) + " not a lambda");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector())) {
				errors.push_back(cata(typeprinter_alg, b.vector().type) + " not a vector");
			}
		}
		if (errors.empty()) {
			auto lambda_type = boost::get<Arrow_t<Fix<TF>>>(b.lambda().type);
			string param_type = cata(typeprinter_alg, lambda_type.left());
			auto vector_type = boost::get<Power_t<Fix<TF>>>(b.vector().type);
			string input_type = cata(typeprinter_alg, vector_type.left());
			if (param_type != input_type) {
				errors.push_back(param_type + " != " + input_type);
			}
			type = errors.empty() ? Power(lambda_type.right(), vector_type.right()) : Invalid();
		}
		return{ b, type, errors };
	}

	typecheck_result operator()(Reduce_<typecheck_t> op) const {
		list<string> errors;
		errors.insert(errors.end(), op.lambda().second.begin(), op.lambda().second.end());
		errors.insert(errors.end(), op.vector().second.begin(), op.vector().second.end());
		Reduce_<FF> b(op.lambda().first, op.vector().first);
		Fix<TF> type;
		if (errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda()) ||
				!is_of_type<Arrow_t<Fix<TF>>>(boost::get<Arrow_t<Fix<TF>>>(b.lambda().type).right())) {
				errors.push_back(cata(typeprinter_alg, b.lambda().type) + " not a binary lambda");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector())) {
				errors.push_back(cata(typeprinter_alg, b.vector().type) + " not a vector");
			}
		}
		if (errors.empty()) {
			auto lambda1_type = boost::get<Arrow_t<Fix<TF>>>(b.lambda().type);
			auto lambda2_type = boost::get<Arrow_t<Fix<TF>>>(lambda1_type.right());
			string param1_type = cata(typeprinter_alg, lambda1_type.left());
			string param2_type = cata(typeprinter_alg, lambda2_type.left());
			string result_type = cata(typeprinter_alg, lambda2_type.right());
			auto vector_type = boost::get<Power_t<Fix<TF>>>(b.vector().type);
			string input_type = cata(typeprinter_alg, vector_type.left());
			if (boost::get<Size_t>(vector_type.right()).size == 0) {
				errors.push_back(cata(typeprinter_alg, b.vector().type) + " is an empty vector");
			}
			if (param1_type != input_type) {
				errors.push_back(param1_type + " != " + input_type);
			}
			if (param2_type != input_type) {
				errors.push_back(param2_type + " != " + input_type);
			}
			if (result_type != input_type) {
				errors.push_back(result_type + " != " + input_type);
			}
			type = errors.empty() ? vector_type.left() : Invalid();
		}
		return{ b, type, errors };
	}

	typecheck_result operator()(Zip_<typecheck_t> op) const {
		list<string> errors;
		errors.insert(errors.end(), op.lambda().second.begin(), op.lambda().second.end());
		errors.insert(errors.end(), op.vector_1().second.begin(), op.vector_1().second.end());
		errors.insert(errors.end(), op.vector_2().second.begin(), op.vector_2().second.end());
		Zip_<FF> b(op.lambda().first, op.vector_1().first, op.vector_2().first);
		Fix<TF> type;
		if (errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda()) ||
				!is_of_type<Arrow_t<Fix<TF>>>(boost::get<Arrow_t<Fix<TF>>>(b.lambda().type).right())) {
				errors.push_back(cata(typeprinter_alg, b.lambda().type) + " not a binary lambda");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector_1())) {
				errors.push_back(cata(typeprinter_alg, b.vector_1().type) + " not a vector");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector_2())) {
				errors.push_back(cata(typeprinter_alg, b.vector_2().type) + " not a vector");
			}
		}
		if (errors.empty()) {
			auto lambda1_type = boost::get<Arrow_t<Fix<TF>>>(b.lambda().type);
			auto lambda2_type = boost::get<Arrow_t<Fix<TF>>>(lambda1_type.right());
			string param1_type = cata(typeprinter_alg, lambda1_type.left());
			string param2_type = cata(typeprinter_alg, lambda2_type.left());
			string result_type = cata(typeprinter_alg, lambda2_type.right());
			auto vector1_type = boost::get<Power_t<Fix<TF>>>(b.vector_1().type);
			string input1_type = cata(typeprinter_alg, vector1_type.left());
			string size1_type = cata(typeprinter_alg, vector1_type.right());
			auto vector2_type = boost::get<Power_t<Fix<TF>>>(b.vector_2().type);
			string input2_type = cata(typeprinter_alg, vector2_type.left());
			string size2_type = cata(typeprinter_alg, vector2_type.right());
			if (param1_type != input1_type) {
				errors.push_back(param1_type + " != " + input1_type);
			}
			if (param2_type != input2_type) {
				errors.push_back(param2_type + " != " + input2_type);
			}
			if (size1_type != size2_type) {
				errors.push_back(size1_type + " != " + size2_type);
			}
			type = errors.empty() ? Power(lambda2_type.right(), vector1_type.right()) : Invalid();
		}
		return{ b, type, errors };
	}

	F<typecheck_t>& node;
};

typecheck_t typecheck_alg(F<typecheck_t> e) {
	typecheck_result result = boost::apply_visitor(typecheck_alg_visitor(e), e.operation);
	return{ Fx(F<FF>{result.operation, result.type}), result.errors };
}
