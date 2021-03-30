cmake_minimum_required(VERSION 3.17)
project(template_programming)

set(CMAKE_CXX_STANDARD 20)

add_executable(template_programming main.cpp collection/iterator.hpp function_tools/function_trait.hpp function_tools/higher_order_function.hpp)