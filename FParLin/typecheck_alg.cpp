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
		return{ Fx((F<Fix<F>>)a), State() };
	}

	typecheck_t operator()(VectorView a) const {
		a.type = Power(Double(), Size(a.size));
		return{ Fx((F<Fix<F>>)a), State() };
	}

	typecheck_t operator()(Vector<typecheck_t> a) const {
		State state;
		for (auto e : a.elements) {
			state.add(e->second);
		}
		vector<Fix<F>> elements;
		transform(a.elements.begin(), a.elements.end(), back_inserter(elements),
			[](shared_ptr<typecheck_t> p) {return p->first; });
		Vector<Fix<F>> b(elements, a.values, a.subst, a.type, a.cost);
		if (state.errors.empty()) {
			string type_str = cata(typeprinter_alg, get_type(*b.elements[0]));
			unsigned i;
			for (i = 1; i < b.elements.size(); ++i) {
				string elem_type = cata(typeprinter_alg, get_type(*b.elements[i]));
				if (elem_type != type_str) {
					state.errors.push_back(type_str + " != " + elem_type);
					break;
				}
			}

			vector<unsigned> dimensions = get_dimensions(Power(get_type(*b.elements[0]), Size((unsigned)b.elements.size())));
			unsigned prod = 1;
			for (int i = 0; i < (int)dimensions.size() - 1; ++i) {
				state.add_vec({ element_type::VECTOR, dimensions[i] }, prod);
				prod *= dimensions[i];
			}
			state.add_vec({ element_type::DOUBLE, dimensions.back() }, prod);
		}
		b.type = state.errors.empty() ? Power(get_type(*b.elements[0]), Size((unsigned)b.elements.size())) : Invalid();
		return{ Fx((F<Fix<F>>)b), state };
	}

	typecheck_t operator()(Addition<typecheck_t> a) const {
		State state;
		state.add(a.left().second).add(a.right().second);
		Addition<Fix<F>> b(a.left().first, a.right().first, a.values, a.subst, a.type, a.cost);
		if (state.errors.empty()) {
			if (!is_of_type<Int_t>(b.left()) && !is_of_type<Double_t>(b.left())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.left())) + " not scalar");
			}
			if (!is_of_type<Int_t>(b.right()) && !is_of_type<Double_t>(b.right())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.right())) + " not scalar");

			}
		}
		b.type = state.errors.empty() ?
			(is_of_type<Int_t>(b.left()) && is_of_type<Int_t>(b.right()) ? Int() : Double()) :
			Invalid();
		return{ Fx((F<Fix<F>>)b), state };
	}

	typecheck_t operator()(Multiplication<typecheck_t> a) const {
		State state;
		state.add(a.left().second).add(a.right().second);
		Multiplication<Fix<F>> b(a.left().first, a.right().first, a.values, a.subst, a.type, a.cost);
		if (state.errors.empty()) {
			if (!is_of_type<Int_t>(b.left()) && !is_of_type<Double_t>(b.left())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.left())) + " not scalar");
			}
			if (!is_of_type<Int_t>(b.right()) && !is_of_type<Double_t>(b.right())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.right())) + " not scalar");

			}
		}
		b.type = state.errors.empty() ?
			(is_of_type<Int_t>(b.left()) && is_of_type<Int_t>(b.right()) ? Int() : Double()) :
			Invalid();
		return{ Fx((F<Fix<F>>)b), state };
	}

	typecheck_t operator()(Apply<typecheck_t> a) const {
		State state;
		state.add(a.lambda().second).add(a.input().second);
		Apply<Fix<F>> b(a.lambda().first, a.input().first, a.values, a.subst, a.type, a.cost);
		if (state.errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.lambda())) + " not a lambda");
			}
			else {
				auto lambda_type = boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda()));
				string param_type = cata(typeprinter_alg, lambda_type.left());
				string input_type = cata(typeprinter_alg, get_type(b.input()));
				if (param_type != input_type) {
					state.errors.push_back(param_type + " != " + input_type);
				}
				b.type = state.errors.empty() ? lambda_type.right() : Invalid();
			}
		}
		return{ Fx((F<Fix<F>>)b), state };
	}

	typecheck_t operator()(Lambda<typecheck_t> a) const {
		State state = a.body().second;
		Lambda<Fix<F>> b(a.body().first, a.id, a.values, a.subst, a.type, a.cost);
		if (state.errors.empty()) {
			auto lambda_type = boost::get<Arrow_t<Fix<TF>>>(b.type);
			string return_type = cata(typeprinter_alg, lambda_type.right());
			string body_type = cata(typeprinter_alg, get_type(b.body()));
			if (return_type != body_type) {
				state.errors.push_back(return_type + " != " + body_type);
			}
			b.type = state.errors.empty() ? b.type : Invalid();
		}
		return{ Fx((F<Fix<F>>)b), state };
	}

	typecheck_t operator()(Variable a) const {
		return{ Fx((F<Fix<F>>)a), State() };
	}

	typecheck_t operator()(Map_<typecheck_t> a) const {
		State state;
		Map_<Fix<F>> b(a.lambda().first, a.vector().first, a.values, a.subst, a.type, a.cost);
		if (state.errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.lambda())) + " not a lambda");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.vector())) + " not a vector");
			}
		}
		if (state.errors.empty()) {
			auto lambda_type = boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda()));
			string param_type = cata(typeprinter_alg, lambda_type.left());
			auto vector_type = boost::get<Power_t<Fix<TF>>>(get_type(b.vector()));
			string input_type = cata(typeprinter_alg, vector_type.left());
			if (param_type != input_type) {
				state.errors.push_back(param_type + " != " + input_type);
			}
			b.type = state.errors.empty() ? Power(lambda_type.right(), vector_type.right()) : Invalid();

			state.add(a.lambda().second, (int)boost::get<Size_t>(vector_type.right()).size)
				.add(a.vector().second);
			vector<unsigned> dimensions = get_dimensions(Power(lambda_type.right(), vector_type.right()));
			unsigned prod = 1;
			for (int i = 0; i < (int)dimensions.size() - 1; ++i) {
				state.add_vec({ element_type::VECTOR, dimensions[i] }, prod);
				prod *= dimensions[i];
			}
			state.add_vec({ element_type::DOUBLE, dimensions.back() }, prod);
		}
		return{ Fx((F<Fix<F>>)b), state };
	}

	typecheck_t operator()(Fold_<typecheck_t> a) const {
		State state;
		state.add(a.lambda().second).add(a.vector().second).add(a.init().second);
		Fold_<Fix<F>> b(a.lambda().first, a.vector().first, a.init().first, a.values, a.subst, a.type, a.cost);
		if (state.errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda()) ||
				!is_of_type<Arrow_t<Fix<TF>>>(boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda())).right())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.lambda())) + " not a binary lambda");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.vector())) + " not a vector");
			}
		}
		if (state.errors.empty()) {
			auto lambda1_type = boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda()));
			auto lambda2_type = boost::get<Arrow_t<Fix<TF>>>(lambda1_type.right());
			string param1_type = cata(typeprinter_alg, lambda1_type.left());
			string param2_type = cata(typeprinter_alg, lambda2_type.left());
			string result_type = cata(typeprinter_alg, lambda2_type.right());
			string init_type = cata(typeprinter_alg, get_type(b.init()));
			auto vector_type = boost::get<Power_t<Fix<TF>>>(get_type(b.vector()));
			string input_type = cata(typeprinter_alg, vector_type.left());
			if (param1_type != init_type) {
				state.errors.push_back(param1_type + " != " + init_type);
			}
			if (param2_type != input_type) {
				state.errors.push_back(param2_type + " != " + input_type);
			}
			if (result_type != init_type) {
				state.errors.push_back(result_type + " != " + init_type);
			}
			b.type = state.errors.empty() ? get_type(b.init()) : Invalid();
		}
		//if (state.errors.empty()) {
		//	vector<unsigned> dimensions = get_dimensions(Power(lambda_type.right(), vector_type.right()));
		//	for (int i = 0; i < (int)dimensions.size() - 1; ++i) {
		//		state.results.push_back({ element_type::VECTOR, dimensions[i] });
		//	}
		//	state.results.push_back({ element_type::DOUBLE, dimensions.back() });
		//}
		return{ Fx((F<Fix<F>>)b), state };
	}

	typecheck_t operator()(Zip_<typecheck_t> a) const {
		State state;
		Zip_<Fix<F>> b(a.lambda().first, a.vector_1().first, a.vector_2().first, a.values, a.subst, a.type, a.cost);
		if (state.errors.empty()) {
			if (!is_of_type<Arrow_t<Fix<TF>>>(b.lambda()) ||
				!is_of_type<Arrow_t<Fix<TF>>>(boost::get<Arrow_t<Fix<TF>>>(get_type(b.lambda())).right())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.lambda())) + " not a binary lambda");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector_1())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.vector_1())) + " not a vector");
			}
			if (!is_of_type<Power_t<Fix<TF>>>(b.vector_2())) {
				state.errors.push_back(cata(typeprinter_alg, get_type(b.vector_2())) + " not a vector");
			}
		}
		if (state.errors.empty()) {
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
				state.errors.push_back(param1_type + " != " + input1_type);
			}
			if (param2_type != input2_type) {
				state.errors.push_back(param2_type + " != " + input2_type);
			}
			if (size1_type != size2_type) {
				state.errors.push_back(size1_type + " != " + size2_type);
			}
			b.type = state.errors.empty() ? Power(lambda2_type.right(), vector1_type.right()) : Invalid();

			state.add(a.lambda().second, (int)boost::get<Size_t>(vector1_type.right()).size)
				.add(a.vector_1().second).add(a.vector_2().second);
			vector<unsigned> dimensions = get_dimensions(Power(lambda2_type.right(), vector1_type.right()));
			unsigned prod = 1;
			for (int i = 0; i < (int)dimensions.size() - 1; ++i) {
				state.add_vec({ element_type::VECTOR, dimensions[i] }, prod);
				prod *= dimensions[i];
			}
			state.add_vec({ element_type::DOUBLE, dimensions.back() }, prod);
		}
		return{ Fx((F<Fix<F>>)b), state };
	}

	vector<unsigned> get_dimensions(Fix<TF> vec) const {
		vector<unsigned> result;
		for (; is_of_type<Power_t<Fix<TF>>>(vec); vec = boost::get<Power_t<Fix<TF>>>(vec).left()) {
			result.push_back(boost::get<Size_t>(boost::get<Power_t<Fix<TF>>>(vec).right()).size);
		}
		return result;
	}
};

typecheck_t typecheck_alg(F<typecheck_t> e) {
	return boost::apply_visitor(typecheck_alg_visitor(), e);
}

State& State::add(const State & other, int mul) {
	errors.insert(errors.end(), other.errors.begin(), other.errors.end());
	for (const auto& vec : other.results) {
		add_vec(vec.first, vec.second * mul);
	}
	return *this;
}

State& State::add_vec(std::pair<element_type, unsigned> vec, int count) {
	auto p = results.insert({ vec,count });
	if (!p.second) {
		(*p.first).second += count;
	}
	return *this;
}
