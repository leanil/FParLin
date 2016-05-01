#include "type.h"

Fix<TF> Invalid() { return Fx(TF<Fix<TF>>(Invalid_t())); }

Fix<TF> Int() { return Fx(TF<Fix<TF>>(Int_t())); }

Fix<TF> Double() { return Fx(TF<Fix<TF>>(Double_t())); }

Fix<TF> operator""_size(unsigned long long x) { return Fx(TF<Fix<TF>>(Size_t{ (int)x })); }

Fix<TF> Size(int x) { return Fx(TF<Fix<TF>>(Size_t{ x })); }