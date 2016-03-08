#include "fparlin.hpp"
#include <iostream>

int main() {
	std::cout << ((2)+(3))*(4) << std::endl;
	std::cout << [&](const auto& x){return [&](const auto& y){return (x)+(y);};}(5)((4)*(3)) << std::endl;
	std::cout << make_vector({((2)+(3))*(4),[&](const auto& x){return [&](const auto& y){return (x)+(y);};}(5)((4)*(3)),((2)+(3))*(4)}) << std::endl;
	std::cout << Map([&](const auto& x){return [&](const auto& y){return (x)+(y);};}(5),make_vector({((2)+(3))*(4),[&](const auto& x){return [&](const auto& y){return (x)+(y);};}(5)((4)*(3)),((2)+(3))*(4)})) << std::endl;
}