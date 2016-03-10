#include "typecheck_alg.h"

#include "falgebra.h"
#include "type.h"
#include "typeprinter_alg.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/static_visitor.hpp>

#include <iostream>

struct alg_visitor : boost::static_visitor<Fix<F>> {

	Fix<F> operator()(Scalar a) const {
		a.type = Fx(Double());
		return Fx((F<Fix<F>>)a);
	}

	Fix<F> operator()(VectorView a) const {
		a.type = Fx(Power(Fx(Double()), Fx(Value(a.size))));
		return Fx((F<Fix<F>>)a);
	}

	Fix<F> operator()(Vector<Fix<F>> a) const {
		a.type = Fx(Power(get_type(*a.elements[1]), Fx(Value(a.elements.size()))));
		return Fx((F<Fix<F>>)a);
	}

	Fix<F> operator()(Addition<Fix<F>> a) const {
		a.type = get_type(a.left());
		return Fx((F<Fix<F>>)a);
	}

	Fix<F> operator()(Multiplication<Fix<F>> a) const {
		a.type = get_type(a.left());
		return Fx((F<Fix<F>>)a);
	}

	Fix<F> operator()(Apply<Fix<F>> a) const {
		//cout << cata(typeprinter_alg, get_type(a.lambda())) << endl;
		//cout << get_type(a.lambda()).which() << endl;
		a.type = boost::get<Arrow_t<Fix<TF>>>(get_type(a.lambda())).right();
		return Fx((F<Fix<F>>)a);
	}

	Fix<F> operator()(Lambda<Fix<F>> a) const {
		return Fx((F<Fix<F>>)a);
	}

	Fix<F> operator()(Variable a) const {
		return Fx((F<Fix<F>>)a);
	}

	Fix<F> operator()(Map_<Fix<F>> a) const {
		a.type = Fx(Power(
			boost::get<Arrow_t<Fix<TF>>>(get_type(a.lambda())).right(),
			boost::get<Power_t<Fix<TF>>>(get_type(a.vector())).right()));
		return Fx((F<Fix<F>>)a);
	}

	Fix<F> operator()(Fold_<Fix<F>> a) const {
		a.type = get_type(a.init());
		return Fx((F<Fix<F>>)a);
	}

	Fix<F> operator()(Zip_<Fix<F>> a) const {
		a.type = Fx(Power(
			boost::get<Arrow_t<Fix<TF>>>(get_type(a.lambda())).right(),
			boost::get<Power_t<Fix<TF>>>(get_type(a.vector_1())).right()));
		return Fx((F<Fix<F>>)a);
	}
};

Fix<F> typecheck_alg(F<Fix<F>> e) {
	return boost::apply_visitor(alg_visitor(), e);
}