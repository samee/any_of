cmake_minimum_required(VERSION 3.22)

project(any_of)
set(CMAKE_CXX_STANDARD 11)
enable_testing()

add_library(any_of INTERFACE any_of.h)

add_executable(any_of_test any_of_test.cpp any_of)
add_test(any_of_test any_of_test)

add_executable(readme_sample readme_sample.cpp any_of)
add_test(readme_sample readme_sample)

install(FILES any_of.h DESTINATION include)
