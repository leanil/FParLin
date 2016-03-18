#include "config.hpp"
#include "fparlin.hpp"
#include <iostream>

int main() {
    std::cout << Map([&](const auto& v){return Fold([&](const auto& x){return [&](const auto& y){return (x)+(y);};},Zip([&](const auto& x){return [&](const auto& y){return (x)*(y);};},v,*bigVectors.at("vec")),0.000000);},make_vector({make_vector({1.000000,2.000000,3.000000}),make_vector({4.000000,5.000000,6.000000})})) << std::endl;
}