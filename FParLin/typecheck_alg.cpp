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

struct typecheck_alg_visitor : boost::static_visitor<typecheck_t> {

	typecheck_t operator()(Scalar a) const {
		a.type = Double();
		return{ Fx((F<Fix<F>>)a), list<string>() };
	}

	typecheck_t operator()(VectorView a) const {
		a.type = Power(Double(), Size(a.size));
		return{ Fx((F<Fix<F>>)a), list<string>() };
	}

	typecheck_t operator()(Vector<typecheck_t> a) const {
		list<string> errors;
		for (auto e : a.elements) {
			errors.insert(errors.end(), e->second.begin(), e->second.end());
		}
		vector<Fix<F>> elements;
		transform(a.elements.begin(), a.elements.end(), back_inserter(elements),
			[](shared_ptr<typecheck_t> p) {return p->first; });
		Vector<Fix<F>> b(elements, a.values, a.subst, a.type, a.cost);
		if (errors.empty()) {
			string type_str = cata(typeprinter_alg, get_type(*b.elements[0]));
			unsigned i;
			for (i = 1; i < b.elements.size(); ++i) {
				string elem_type = cata(typeprinter_alg, get_type(*b.elements[i]));
				if (elem_type != type_str) {
					errors.push_back(type_str + " != " + elem_type);
					break;
				}
			}
		}
		b.type = errors.empty() ? Power(get_type(*b.elements[0]), Size((unsigned)b.elements.size())) : Invalid();
		return{ Fx((F<Fix<F>>)b), errors };
	}

	typecheck_t operator()(Addition<typecheck_t> a) const {
		list<string> errors;
		errors.insert(errors.end(), a.left().second.begin(), a.left().second.end());
		errors.insert(errors.end(), a.right().second.begin(), a.right().second.end());
		Addition<Fix<F>> b(a.left().first, a.right().first, a.values, a.subst, a.type, a.cost);
		if (errors.empty()) {
			if (!is_of_type<Int_t>(b.left()) && !is_of_type<Double_t>(b.left())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.left())) + " not scalar");
			}
			if (!is_of_type<Int_t>(b.right()) && !is_of_type<Double_t>(b.right())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.right())) + " not scalar");

			}
		}
		b.type = errors.empty() ?
			(is_of_type<Int_t>(b.left()) && is_of_type<Int_t>(b.right()) ? Int() : Double()) :
			Invalid();
		return{ Fx((F<Fix<F>>)b), errors };
	}

	typecheck_t operator()(Multiplication<typecheck_t> a) const {
		list<string> errors;
		errors.insert(errors.end(), a.left().second.begin(), a.left().second.end());
		errors.insert(errors.end(), a.right().second.begin(), a.right().second.end());
		Multiplication<Fix<F>> b(a.left().first, a.right().first, a.values, a.subst, a.type, a.cost);
		if (errors.empty()) {
			if (!is_of_type<Int_t>(b.left()) && !is_of_type<Double_t>(b.left())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.left())) + " not scalar");
			}
			if (!is_of_type<Int_t>(b.right()) && !is_of_type<Double_t>(b.right())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.right())) + " not scalar");

			}
		}
		b.type = errors.empty() ?
			(is_of_type<Int_t>(b.left()) && is_of_type<Int_t>(b.right()) ? Int() : Double()) :
			Invalid();
		return{ Fx((F<Fix<F>>)b), errors };
	}

	typecheck_t operator()(Apply<typecheck_t> a) const {
		list<string> errors;
		errors.insert(errors.end(), a.lambda().second.begin(), a.lambda().second.end());
		errors.insert(errors.end(), a.input().second.begin(), a.input().second.end());
		Apply<Fix<F>> b(a.lambda().first, a.input().first, a.values, a.subst, a.type, a.cost);
		if (errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.lambda())) + " not a lambda");
			}
			else {
				auto lambda_type = boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda()));
				string param_type = cata(typeprinter_alg, lambda_type.left());
				string input_type = cata(typeprinter_alg, get_type(b.input()));
				if (param_type != input_type) {
					errors.push_back(param_type + " != " + input_type);
				}
				b.type = errors.empty() ? lambda_type.right() : Invalid();
			}
		}	
		return{ Fx((F<Fix<F>>)b), errors };
	}

	typecheck_t operator()(Lambda<typecheck_t> a) const {
		list<string> errors(a.body().second);
		Lambda<Fix<F>> b(a.body().first, a.id, a.values, a.subst, a.type, a.cost);
		if (errors.empty()) {
			auto lambda_type = boost::get<Arrow_t<Fix<TF>>>(b.type);
			string return_type = cata(typeprinter_alg, lambda_type.right());
			string body_type = cata(typeprinter_alg, get_type(b.body()));
			if (return_type != body_type) {
				errors.push_back(return_type + " != " + body_type);
			}
			b.type = errors.empty() ? b.type : Invalid();
		}
		return{ Fx((F<Fix<F>>)b), errors };
	}

	typecheck_t operator()(Variable a) const {
		return{ Fx((F<Fix<F>>)a), list<string>() };
	}

	typecheck_t operator()(Map_<typecheck_t> a) const {
		list<string> errors;
		errors.insert(errors.end(), a.lambda().second.begin(), a.lambda().second.end());
		errors.insert(errors.end(), a.vector().second.begin(), a.vector().second.end());
		Map_<Fix<F>> b(a.lambda().first, a.vector().first, a.values, a.subst, a.type, a.cost);
		if (errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.lambda())) + " not a lambda");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.vector())) + " not a vector");
			}
		}
		if(errors.empty()) {
				auto lambda_type = boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda()));
				string param_type = cata(typeprinter_alg, lambda_type.left());
				auto vector_type = boost::get<Power_t<Fix<TF>>>(get_type(b.vector()));
				string input_type = cata(typeprinter_alg, vector_type.left());
				if (param_type != input_type) {
					errors.push_back(param_type + " != " + input_type);
				}
				b.type = errors.empty() ? Power(lambda_type.right(), vector_type.right()) : Invalid();
		}
		return{ Fx((F<Fix<F>>)b), errors };
	}

	typecheck_t operator()(Reduce_<typecheck_t> a) const {
		list<string> errors;
		errors.insert(errors.end(), a.lambda().second.begin(), a.lambda().second.end());
		errors.insert(errors.end(), a.vector().second.begin(), a.vector().second.end());
		Reduce_<Fix<F>> b(a.lambda().first, a.vector().first, a.values, a.subst, a.type, a.cost);
		if (errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda()) ||
				!is_of_type<Arrow_t<Fix<TF>>>(boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda())).right())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.lambda())) + " not a binary lambda");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.vector())) + " not a vector");
			}
		}
		if (errors.empty()) {
			auto lambda1_type = boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda()));
			auto lambda2_type = boost::get<Arrow_t<Fix<TF>>>(lambda1_type.right());
			string param1_type = cata(typeprinter_alg, lambda1_type.left());
			string param2_type = cata(typeprinter_alg, lambda2_type.left());
			string result_type = cata(typeprinter_alg, lambda2_type.right());
			auto vector_type = boost::get<Power_t<Fix<TF>>>(get_type(b.vector()));
			string input_type = cata(typeprinter_alg, vector_type.left());
			if (boost::get<Size_t>(vector_type.right()).size == 0) {
				errors.push_back(cata(typeprinter_alg, get_type(b.vector())) + " is an empty vector");
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
			b.type = errors.empty() ? vector_type.left() : Invalid();
		}
		return{ Fx((F<Fix<F>>)b), errors };
	}

	typecheck_t operator()(Zip_<typecheck_t> a) const {
		list<string> errors;
		errors.insert(errors.end(), a.lambda().second.begin(), a.lambda().second.end());
		errors.insert(errors.end(), a.vector_1().second.begin(), a.vector_1().second.end());
		errors.insert(errors.end(), a.vector_2().second.begin(), a.vector_2().second.end());
		Zip_<Fix<F>> b(a.lambda().first, a.vector_1().first, a.vector_2().first, a.values, a.subst, a.type, a.cost);
		if (errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda()) ||
				!is_of_type<Arrow_t<Fix<TF>>>(boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda())).right())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.lambda())) + " not a binary lambda");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector_1())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.vector_1())) + " not a vector");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector_2())) {
				errors.push_back(cata(typeprinter_alg, get_type(b.vector_2())) + " not a vector");
			}
		}
		if (errors.empty()) {
			auto lambda1_type = boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda()));
			auto lambda2_type = boost::get<Arrow_t<Fix<TF>>>(lambda1_type.right());
			string param1_type = cata(typeprinter_alg, lambda1_type.left());
			string param2_type = cata(typeprinter_alg, lambda2_type.left());
			string result_type = cata(typeprinter_alg, lambda2_type.right());
			auto vector1_type = boost::get<Power_t<Fix<TF>>>(get_type(b.vector_1()));
			string input1_type = cata(typeprinter_alg, vector1_type.left());
			string size1_type = cata(typeprinter_alg, vector1_type.right());
			auto vector2_type = boost::get<Power_t<Fix<TF>>>(get_type(b.vector_2()));
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
			b.type = errors.empty() ? Power(lambda2_type.right(), vector1_type.right()) : Invalid();
		}
		return{ Fx((F<Fix<F>>)b), errors };
	}
};

typecheck_t typecheck_alg(F<typecheck_t> e) {
	return boost::apply_visitor(typecheck_alg_visitor(), e);
}
