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

	void operator()(Scalar& i) const {}

	void operator()(Vector<Fix<F>>& e) const {
		e.values = values;
		e.subst = subst;
	}

	void operator()(Addition<Fix<F>>& e) const {
		e.values = values;
		e.subst = subst;
	}

	void operator()(Multiplication<Fix<F>>& e) const {
		e.values = values;
		e.subst = subst;
	}

	void operator()(Apply<Fix<F>>& e) const {
		e.values = values;
		e.subst = subst;
	}

	void operator()(Lambda<Fix<F>>& e) const {
		e.values = values;
		e.subst = subst;
	}

	void operator()(Variable& e) const {
		e.value = subst.at(e.id);
	}

	const stack<shared_ptr<Fix<F>>>& values;
	const map<char, shared_ptr<Fix<F>>>& subst;
};

struct coalg_visitor : boost::static_visitor<F<Fix<F>>> {

	F<Fix<F>> operator()(Scalar i) const {
		return Scalar{ i.value };
	}

	F<Fix<F>> operator()(Vector<Fix<F>> e) const {
		annotation_copy_visitor copy(e.values, e.subst);
		for (auto element : e.elements) {
			boost::apply_visitor(copy, *element);
		}
		return e;
	}

	F<Fix<F>> operator()(Addition<Fix<F>> e) const {
		annotation_copy_visitor copy(e.values, e.subst);
		boost::apply_visitor(copy, e.left());
		boost::apply_visitor(copy, e.right());
		return e;
	}

	F<Fix<F>> operator()(Multiplication<Fix<F>> e) const {
		annotation_copy_visitor copy(e.values, e.subst);
		boost::apply_visitor(copy, e.left());
		boost::apply_visitor(copy, e.right());
		return e;
	}

	F<Fix<F>> operator()(Apply<Fix<F>> e) const {
		annotation_copy_visitor copy(e.values, e.subst);
		boost::apply_visitor(copy, e.input());
		e.values.push(e.input_);
		boost::apply_visitor(copy, e.lambda());
		return e;
	}

	F<Fix<F>> operator()(Lambda<Fix<F>> e) const {
		e.subst[e.id] = e.values.top();
		e.values.pop();
		annotation_copy_visitor copy(e.values, e.subst);
		boost::apply_visitor(copy, e.body());
		return e;
	}

	F<Fix<F>> operator()(Variable e) const {
		return e;
	}
};

F<Fix<F>> coalg(Fix<F> e) {
	auto n = ((F<Fix<F>>)e);
	return boost::apply_visitor(coalg_visitor(), n);
}