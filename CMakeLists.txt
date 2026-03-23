cmake_minimum_required(VERSION 3.10)
project(projet_algo_s2 C)

set(CMAKE_C_STANDARD 11)

add_executable(projet_algo_s2
        src/main.c)

target_link_libraries(projet_algo_s2 -lmingw32 -lalleg44)
