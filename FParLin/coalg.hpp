#pragma once

#include "expr.hpp"
#include "fmap.hpp"
#include <boost\variant\apply_visitor.hpp>
#include <functional>
#include <type_traits>

template<typename T> struct TemplateGetter;

template< template<typename> class Te, typename OldType > struct TemplateGetter<Te<OldType>> {
	using result = Fix<Te>;
};

template<typename CoAlg, typename A>
typename TemplateGetter< typename std::result_of<CoAlg(A)>::type >::result
ana(CoAlg coalg, A o) {
	using std::placeholders::_1;
	return Fx(fmap(std::bind(&ana<CoAlg, A>, coalg, _1), coalg(o)));
}

struct annotation_copy_visitor : boost::static_visitor<void> {

	annotation_copy_visitor(const stack<shared_ptr<Fix<F>>>& values, const map<char, shared_ptr<Fix<F>>>& subst):
		values{ values }, subst{ subst } {}

	void operator()(Const_& i) const {}

	void operator()(Add_<Fix<F>>& e) const {
		e.values = values;
		e.subst = subst;
	}

	void operator()(Mul_<Fix<F>>& e) const {
		e.values = values;
		e.subst = subst;
	}

	void operator()(App_<Fix<F>>& e) const {
		e.values = values;
		e.subst = subst;
	}

	void operator()(Lambda_<Fix<F>>& e) const {
		e.values = values;
		e.subst = subst;
	}

	void operator()(Variable_& e) const {
		e.value = subst.at(e.id);
	}

	const stack<shared_ptr<Fix<F>>>& values;
	const map<char, shared_ptr<Fix<F>>>& subst;
};

struct coalg_visitor : boost::static_visitor<F<Fix<F>>> {

	F<Fix<F>> operator()(Const_ i) const {
		return Const_{ i.value };
	}

	F<Fix<F>> operator()(Add_<Fix<F>> e) const {
		annotation_copy_visitor copy(e.values, e.subst);
		boost::apply_visitor(copy, *static_cast<F<Fix<F>>*>(e.left_.get()));
		boost::apply_visitor(copy, *static_cast<F<Fix<F>>*>(e.right_.get()));
		return e;
	}

	F<Fix<F>> operator()(Mul_<Fix<F>> e) const {
		annotation_copy_visitor copy(e.values, e.subst);
		boost::apply_visitor(copy, *static_cast<F<Fix<F>>*>(e.left_.get()));
		boost::apply_visitor(copy, *static_cast<F<Fix<F>>*>(e.right_.get()));
		return e;
	}

	F<Fix<F>> operator()(App_<Fix<F>> e) const {
		annotation_copy_visitor copy(e.values, e.subst);
		boost::apply_visitor(copy, *static_cast<F<Fix<F>>*>(e.input_.get()));
		e.values.push(e.input_);
		boost::apply_visitor(copy, *static_cast<F<Fix<F>>*>(e.function_.get()));
		return e;
	}

	F<Fix<F>> operator()(Lambda_<Fix<F>> e) const {
		e.subst[e.id] = e.values.top();
		e.values.pop();
		annotation_copy_visitor copy(e.values, e.subst);
		boost::apply_visitor(copy, *static_cast<F<Fix<F>>*>(e.body_.get()));
		return e;
	}

	F<Fix<F>> operator()(Variable_ e) const {
		return e;
	}
};

F<Fix<F>> coalg(Fix<F> e) {
	auto n = ((F<Fix<F>>)e);
	return boost::apply_visitor(coalg_visitor(), n);
}