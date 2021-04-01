#include <iostream>
#include "collection/iterator.hpp"

using namespace std;

int main() {
    std::vector<int> vs = {1,2,3,4,5,6};
    auto result = IteratorWrapper(vs)
            .filter([](int value){return value % 2 == 0;})
            .map([](int value){return value * 10;})
            .collection();
    for(auto v: result){
        std::cout<<v<<std::endl;
    }
    return 0;
}
