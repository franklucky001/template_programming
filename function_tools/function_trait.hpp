//
// Created by frank guo on 2021/3/19.
//

#ifndef TEMPLATE_PROGRAMMING_FUNCTION_TRAIT_HPP
#define TEMPLATE_PROGRAMMING_FUNCTION_TRAIT_HPP
#include <functional>

// for lambda expression, and functor object, after capturing,
// forward lambda expression to
//      `function_traits<Return (Class::*)(Args...) const>`,
// forward functor object to
//      `struct function_traits<Return (Class::*)(Args...)>`
template <typename Functor>
struct function_traits
        : public function_traits<decltype(&Functor::operator())> {};

// for std::function, ordinary functions and static member functions.
template <typename Return, typename... Args>
struct function_traits<Return(Args...)> {
    using func_type = std::function<Return(Args...)>;
    using return_type = Return;
};

// for function pointer. e.g, &f.
template <typename Return, typename... Args>
struct function_traits<Return(*)(Args...)> {
    using func_type = std::function<Return(Args...)>;
    using return_type = Return;
};

// for capturing functor object (callable struct, struct with overloaded `()`
// operator).
template <typename Class, typename Return, typename... Args>
struct function_traits<Return(Class::*)(Args...)> {
    using func_type = std::function<Return(Args...)>;
    using return_type = Return;
};

// for capturing lambda expression.
template <typename Class, typename Return, typename... Args>
struct function_traits<Return(Class::*)(Args...) const> {
    using func_type = std::function<Return(Args...)>;
    using return_type = Return;
};
#endif //TEMPLATE_PROGRAMMING_FUNCTION_TRAIT_HPP
