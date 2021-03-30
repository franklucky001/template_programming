#include <iostream>
#include "collection/iterator.hpp"

using namespace std;

int main() {
    std::vector<int> vs = {1,2,3,4,5,6};
    std::for_each(vs.begin(), vs.end(), [](auto value){std::cout<<value<<' ';});
    std::cout<<std::endl;
    auto plus_10 = [](int value){return value * 10;};
    auto filter_2 = [](int value){return value % 2 == 0;};
    auto iter = IteratorWrapper(vs)
            .filter(filter_2)
            .map(plus_10);
    for(auto i : iter){
        std::cout<<i<<std::endl;
    }
    return 0;
}
