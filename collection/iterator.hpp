//
// Created by frank guo on 2021/3/19.
//

#ifndef TEMPLATE_PROGRAMMING_ITERATOR_HPP
#define TEMPLATE_PROGRAMMING_ITERATOR_HPP
#include <algorithm>
#include <vector>
//#include "../function_tools/function_trait.hpp"

using namespace std;

template<typename T, typename Fn> struct MapIterator;
template<typename  T, typename Predicate> struct FilterIterator;

template<typename T>
struct Iterator {
    using value_type = T;
    using Self =  Iterator<T>;
    Iterator() = default;
    Iterator(const Self& other) = default;
    Iterator(Self&& other)  noexcept = default;
    virtual ~Iterator() = default;
    class InnerIter {
    protected:
        Self & _inner_iter;
        size_t _index;
    public:
        InnerIter(Self & iter, size_t n): _inner_iter(iter), _index(n){

        }
        virtual bool operator != (const  InnerIter& other){
            return other._index != this->_index;
        }

        virtual value_type& operator* (){
            return this->_inner_iter.get(_index);
        }

        virtual const InnerIter& operator++(){
            ++_index;
            return *this;
        }
    };
    virtual InnerIter& begin() = 0;
    virtual InnerIter& end() = 0;
    virtual size_t size() = 0;
    virtual value_type & get(size_t idx) = 0;
    virtual bool test(value_type& value) {return true;}
    template<typename Fn>
    auto map(Fn && f)->MapIterator<T, decltype(f)>{
        using Return = typename std::result_of<Fn(T)>::type;
        return MapIterator<Return , decltype(f)>(*this, std::forward<decltype(f)>(f));
    }
    template<typename Predicate>
    auto filter(Predicate && predicate) ->FilterIterator<T, decltype(predicate)>{
        return FilterIterator<T, decltype(predicate)>(*this, std::forward<decltype(predicate)>(predicate));
    }
    virtual std::vector<value_type> collection() = 0;
};
template <typename Container>
struct IteratorWrapper : public Iterator<typename Container::value_type>{
    using Self = IteratorWrapper<Container>;
    using Super = Iterator<typename Container::value_type>;
    using std_iterator = typename Container::iterator;
    using value_type = typename Container::value_type;

    class InnerIter : public Super::InnerIter{
        public:
            InnerIter(Super& _iter, size_t n): Super::InnerIter(_iter, n){}
            bool operator != (const  InnerIter& other){
                return other._index != this->_index;
            }
            value_type& operator* (){
                return this->_inner_iter.get(this->_index);
            }
            const InnerIter& operator++(){
                ++this->_index;
                return *this;
            }
    };

    IteratorWrapper() = default;
    explicit IteratorWrapper(Container& container) noexcept{
        this->_first = std::begin(container);
        this->_last = std::end(container);
        this->_size = this->_last - this->_first;
    }
    IteratorWrapper(const Self& other) noexcept{
        this->_first = other._first;
        this->_last = other._last;
        this->_size = this->_last - this->_first;
    }
    IteratorWrapper(Self&& other)noexcept{
        this->_first = other._first;
        this->_last = other._last;
        this->_size = this->_last - this->_first;
    }
    virtual size_t size(){
        return this->_size;
    }
    virtual value_type & get(size_t idx) {
        return *(this->_first + idx);
    };
    InnerIter&  begin(){
        return InnerIter(*this, 0);
    }
    InnerIter&  end(){
        return InnerIter(*this, _size);
    }
    virtual std::vector<value_type> collection() {
        std::vector<value_type> result;
        for(const auto v: *this){
            result.push_back(v);
        }
        return result;
    }
private:
    size_t _size;
    std_iterator _first;
    std_iterator _last;
};

template<typename T, typename Fn>
struct MapIterator : public Iterator<T>{
    using value_type = T;
    using Self = MapIterator<T, Fn>;
    using Super = Iterator<T>;
    class InnerIter : public Super::InnerIter{
    public:
        InnerIter(Super& _iter, size_t n): Super::InnerIter(_iter, n){}
        bool operator != (const  InnerIter& other){
            return other._index != this->_index;
        }
        value_type& operator* (){
            return this->_inner_iter.get(this->_index);
        }
        const InnerIter& operator++(){
            ++this->_index;
            return *this;
        }
    };
    MapIterator(Iterator<T>& iterator, Fn  && f): _iter(iterator), _transform(std::forward<decltype(f)>(f)){

    }
    MapIterator(const Self & other){
        this->_iter = other._iter;
        this->_transform = other._transform;
    }
    MapIterator(Self&& other) noexcept {
        this->_iter = other._iter;
        this->_transform = other._transform;
    }
    size_t size(){
        return this->_iter.size();
    }
    virtual value_type & get(size_t idx) {
        auto value = this->_iter.get(idx);
        return this->_transform(value);
    };
    bool test(value_type& value){
        return this->_iter.test(value);
    }
    InnerIter&  begin(){
        return InnerIter(*this, 0);
    }
    InnerIter&  end(){
        return InnerIter(*this, this->_iter.size());
    }
    virtual std::vector<value_type> collection() {
        std::vector<value_type> result;
        for(const auto v: *this){
            result.push_back(v);
        }
        return result;
    }
private:
    Iterator<T> & _iter;
    Fn _transform;
};
template<typename  T,  typename Predicate>
struct FilterIterator: public Iterator<T>{
    using value_type = T;
    using Self = FilterIterator<T, Predicate>;
    using Super = Iterator<T>;
    class InnerIter : public Super::InnerIter{
    public:
        InnerIter(Super& _iter, size_t n): Super::InnerIter(_iter, n){}
        bool operator != (const  InnerIter& other){
            return other._index != this->_index;
        }
        value_type& operator* (){
            return this->_inner_iter.get(this->_index);
        }
        const InnerIter& operator++(){
            ++this->_index;
            auto inner = this->_inner_iter;
            auto cur=*this;
            while (cur != inner.end() && inner.test(*cur)){
                ++this ->_index;
                cur = *this;
            }
            return *this;
        }
    };
    explicit FilterIterator(Iterator<T>& iterator, Predicate && predicate):_iter(iterator), _predicate(std::forward<decltype(predicate)>(predicate)){
    }
    FilterIterator(const Self& other):_iter(other._iter){
        this->_predicate = std::forward<Predicate>(_predicate);
    }
    FilterIterator(Self&& other) noexcept :_iter(other._iter){
        this->_predicate = std::forward<Predicate>(_predicate);
    }
    size_t size(){
        size_t n = 0;
        for (auto cur= this->begin(); cur != this->end(); ++cur){
            if(this->_predicate(*cur))
                ++n;
        }
        return n;
    }
    virtual value_type & get(size_t idx) {
        value_type  value;
        for(int i=0, j=0; j<idx;++i){
            value = this->_iter.get(j);
            if(this->_predicate(value))++j;
        }
        return value;
    };
    bool test(value_type& value){
        return this->_predicate(value) && this->_iter.test(value);
    }
    InnerIter&  begin(){
        return InnerIter(*this, 0);
    }
    InnerIter&  end(){
        return InnerIter(*this, this->_iter.size());
    }
    virtual std::vector<value_type> collection() {
        std::vector<value_type> result;
        for(const auto v: *this){
            result.push_back(v);
        }
        return result;
    }
private:
   Iterator<T> & _iter;
   Predicate _predicate;
};

template<typename T, size_t n>
auto make_array_iterator(T (&array)[n], size_t pos=0)-> IteratorWrapper<std::vector<T>> {
    auto vec = std::vector<T>(std::begin(array)+pos, std::end(array));
    return IteratorWrapper<std::vector<T>>(vec);
}
#endif //TEMPLATE_PROGRAMMING_ITERATOR_HPP
