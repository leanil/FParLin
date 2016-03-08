#include "fparlin.hpp"
#include <iostream>

int main() {
	std::cout << ((2)+(3))*(4) << std::endl;
	std::cout << [&](const auto& x){return [&](const auto& y){return (x)+(y);};}(5)((4)*(3)) << std::endl;
	std::cout << make_vector({((2)+(3))*(4),[&](const auto& x){return [&](const auto& y){return (x)+(y);};}(5)((4)*(3)),((2)+(3))*(4)}) << std::endl;
	std::cout << Map([&](const auto& x){return [&](const auto& y){return (x)+(y);};}(5),make_vector({((2)+(3))*(4),[&](const auto& x){return [&](const auto& y){return (x)+(y);};}(5)((4)*(3)),((2)+(3))*(4)})) << std::endl;
	std::cout << Map([&](const auto& a){return Fold([&](const auto& b){return [&](const auto& c){return (b)+(c);};},Zip([&](const auto& b){return [&](const auto& c){return (b)*(c);};},a,make_vector({7,8,9})),0);},make_vector({make_vector({1,2,3}),make_vector({4,5,6})})) << std::endl;
}