//
// Created by frank guo on 2021/3/19.
//

#ifndef TEMPLATE_PROGRAMMING_HIGHER_ORDER_FUNCTION_HPP
#define TEMPLATE_PROGRAMMING_HIGHER_ORDER_FUNCTION_HPP
#include "function_trait.hpp"
template<typename Return>
auto curry_impl(std::function<Return()> & f) {
    return std::forward<decltype(f)>(f);
}

template<typename Return, typename Arg>
auto curry_impl(std::function<Return(Arg)> & f) {
    return std::forward<decltype(f)>(f);
}

template<typename Return, typename Arg, typename... Args>
auto curry_impl(std::function<Return(Arg, Args...)> & f) {
    return[f = std::forward<decltype(f)>(f)](Arg && arg) {
        std::function<Return(Args...)> rest = [&f, &arg](Args... args) -> Return {
            return f(std::forward<Arg>(arg), std::forward<Args>(args)...);
        };
        return curry_impl(rest);
    };
}
template<typename F>
auto curry(F const & f) {
    typename function_traits<F>::func_type _f = f;
    return curry_impl(_f);
}

#endif //TEMPLATE_PROGRAMMING_HIGHER_ORDER_FUNCTION_HPP
