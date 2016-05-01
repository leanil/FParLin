#include "typeprinter_alg.h"
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

struct alg_visitor : boost::static_visitor<string> {

	string operator()(Invalid_t) const {
		return "invalid";
	}

	string operator()(Int_t) const {
		return "int";
	}

	string operator()(Double_t) const {
		return "double";
	}

	string operator()(Size_t a) const {
		return to_string(a.size);
	}

	string operator()(Product_t<string> a) const {
		return "(" + a.left() + ")*(" + a.right() + ")";
	}

	string operator()(Power_t<string> a) const {
		return "(" + a.left() + ")^(" + a.right() + ")";
	}

	string operator()(Arrow_t<string> a) const {
		return "(" + a.left() + ")->(" + a.right() + ")";
	}
};

string typeprinter_alg(TF<string> e) {
	return boost::apply_visitor(alg_visitor(), e);
}