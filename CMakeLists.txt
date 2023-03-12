cmake_minimum_required(VERSION 3.24)
project(lox C)

set(CMAKE_C_STANDARD 17)

add_executable(lox main.c lexer.c lexer.h)
