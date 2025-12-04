cmake_minimum_required(VERSION 3.10)
project(baseball_file C)

set(CMAKE_C_STANDARD 11)

add_executable(baseball_file
        baseball_file.c
        baseball_func.c
        test_prod.c
)
