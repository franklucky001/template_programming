#include <iostream>
#include "collection/iterator.hpp"

using namespace std;

int main() {
    std::vector<int> vs = {1,2,3,4,5,6};
    auto plus_10 = [](int value){return value * 10;};
    auto filter_2 = [](int value){return value % 2 == 0;};
    auto result = IteratorWrapper(vs)
            .filter(filter_2)
            .map(plus_10)
            .collection();
    for(auto v: result){
        std::cout<<v<<std::endl;
    }
    return 0;
}
