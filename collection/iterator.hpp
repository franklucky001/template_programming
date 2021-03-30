//
// Created by frank guo on 2021/3/19.
//

#ifndef TEMPLATE_PROGRAMMING_ITERATOR_HPP
#define TEMPLATE_PROGRAMMING_ITERATOR_HPP
#include <algorithm>
#include <vector>
#include "../function_tools/function_trait.hpp"

using namespace std;

template<typename T, typename IterType, typename Fn> struct MapIterator;
template<typename  T, typename IterType, typename Predicate> struct FilterIterator;

template<typename T, typename IterType>
struct Iterator {
    using value_type = T;
    Iterator() = default;
    Iterator(const Iterator<T, IterType>& other) = default;
    Iterator(Iterator<T, IterType>&& other)  noexcept = default;
    virtual ~Iterator() = default;

    virtual Iterator<T, IterType>& operator++()  {
        if(this->current() != this->end()){
            auto cur = this->current();
            ++cur;
        }
        return *this;
    }
    virtual value_type operator*(){
        auto cur = this->current();
        return *cur;
    }
    bool operator != (const Iterator<T, IterType>& other){
        return this->begin() != other.begin() && this->end() != other.end() && this->current() != other.current();
    }

    virtual IterType begin() = 0;
    virtual IterType current() = 0;
    virtual IterType end() = 0;
    template<typename Fn>
    auto map(Fn && f)->MapIterator<typename std::result_of_t<Fn(T)>, IterType, decltype(f)>{
        using Return = typename std::result_of_t<Fn(T)>;
        return MapIterator<Return, IterType, decltype(f)>(*this, std::forward<decltype(f)>(f));
    }

    template<typename Predicate>
    auto filter(Predicate && predicate) ->FilterIterator<T, IterType, decltype(predicate)>{
        return FilterIterator<T, IterType, decltype(predicate)>(*this, std::forward<decltype(predicate)>(predicate));
    }
};
template <typename Container>
struct IteratorWrapper : public Iterator<typename Container::value_type, typename Container::iterator>{
    using IterType = typename Container::iterator;
//    using value_type = typename Container::value_type;
    IteratorWrapper() = default;
    explicit IteratorWrapper(Container& container) noexcept{
        this->_first = std::begin(container);
        this->_last = std::end(container);
        this->_cur = this->_first;
    }
    IteratorWrapper(const IteratorWrapper<Container>& other) noexcept{
        this->_first = other._first;
        this->_last = other._last;
        this->_cur = other._cur;
    }
    IteratorWrapper(IteratorWrapper<Container>&& other)noexcept{
        this->_first = other._first;
        this->_last = other._last;
        this->_index = other._index;
    }
    IteratorWrapper<Container>& operator= (const IteratorWrapper<Container>& other){
        this->_first = other._first;
        this->_last = other._last;
        this->_index = other._index;
        return *this;
    }
    IteratorWrapper<Container>& operator= (IteratorWrapper<Container>&& other)noexcept{
        this->_first = other._first;
        this->_last = other._last;
        this->_index = other._index;
        return *this;
    }
    IterType  begin(){
        return this->_first;
    }
    IterType  current(){
        return this->_cur;
    }
    IterType  end(){
        return this->_last;
    }
private:
    IterType _cur;
    IterType _first;
    IterType _last;
};

template<typename T, typename IterType, typename Fn>
struct MapIterator : public Iterator<T, IterType>{

    using value_type = T;
    MapIterator(Iterator<T, IterType>& iterator, Fn  & f): _iter(iterator), _transform(std::forward<decltype(f)>(f)){
    }
    MapIterator(const MapIterator<T, IterType, Fn> & other){
        this->_iter = other._iter;
        this->_transform = other._transform;
    }
    MapIterator(MapIterator<T, IterType, Fn>&& other) noexcept {
        this->_iter = other._iter;
        this->_transform = other._transform;
    }
    value_type operator*(){
        return _transform(*(this->_iter));
    }
    IterType  begin(){
        return this->_iter.begin();
    }
    IterType  current(){
        return this->_iter.current();
    }
    IterType  end(){
        return this->_iter.end();
    }
private:
    Iterator<T, IterType> & _iter;
    Fn _transform;
};
template<typename  T, typename IterType, typename Predicate>
struct FilterIterator: public Iterator<T, IterType>{

    explicit FilterIterator(Iterator<T, IterType>& iterator, Predicate & predicate):_iter(iterator), _predicate(std::forward<decltype(predicate)>(predicate)){
    }
    FilterIterator(const FilterIterator<T, IterType, Predicate>& other):_iter(other._iter){
        this->_predicate = std::forward<Predicate>(_predicate);
    }
    FilterIterator(FilterIterator<T, IterType, Predicate>&& other) noexcept :_iter(other._iter){
        this->_predicate = std::forward<Predicate>(_predicate);
    }
    virtual FilterIterator<T, IterType, Predicate>& operator++(){
        auto cur = this->current();
        ++cur;
        for(; !this->_predicate(*cur) && cur != this->end(); ++cur);
        return *this;
    }
    IterType  begin(){
        return this->_iter.begin();
    }
    IterType  current(){
        return this->_iter.current();
    }
    IterType  end(){
        return this->_iter.end();
    }
private:
   Iterator<T, IterType> & _iter;
   Predicate _predicate;
};

template<typename T, size_t n>
auto make_array_iterator(T (&array)[n], size_t pos=0)-> IteratorWrapper<std::vector<T>> {
    auto vec = std::vector<T>(std::begin(array)+pos, std::end(array));
    return IteratorWrapper<std::vector<T>>(vec);
}
#endif //TEMPLATE_PROGRAMMING_ITERATOR_HPP
