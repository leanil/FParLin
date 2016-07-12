#include "type.h"

Fix<TF> Invalid() { return Fx<TF>(Invalid_t()); }

Fix<TF> Int() { return Fx<TF>(Int_t()); }

Fix<TF> Double() { return Fx<TF>(Double_t()); }

Fix<TF> operator""_size(unsigned long long x) { return Fx<TF>(Size_t{ (unsigned)x }); }

Fix<TF> Size(unsigned x) { return Fx<TF>(Size_t{ x }); }

Fix<TF> Power(Fix<TF> a, Fix<TF> b) { return Fx<TF>(Power_t<Fix<TF>>(a, b)); }

Fix<TF> Arrow(Fix<TF> a, Fix<TF> b) { return Fx<TF>(Arrow_t<Fix<TF>>(a, b)); }