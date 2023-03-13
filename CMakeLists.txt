cmake_minimum_required(VERSION 3.24)
project(lox C)

set(CMAKE_C_STANDARD 17)

add_executable(lox src/main.c src/lexer.c src/lexer.h src/identifier.c src/identifier.h src/base.h)
