#pragma once

template<template<typename> class T>
struct Fix : T<Fix<T>> {
	Fix() {}
	explicit Fix(const T<Fix<T>>& t) : T<Fix<T>>(t) {}
};

template<template<typename> class T>
Fix<T> Fx(T<Fix<T>> t) {
	return Fix<T>{t};
}

template<template<typename> class T>
T<Fix<T>>& unFix(Fix<T>& f) {
	return f;
}