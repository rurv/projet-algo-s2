cmake_minimum_required(VERSION 3.10)
project(projet_algo_s2 C)

set(CMAKE_C_STANDARD 11)
set(ALLEGRO_PATH "C:/Windows/SysWOW64/alleg44.dll")

include_directories("C:/MinGW avec Allegro 4/MinGW/include")

add_library(allegro_lib SHARED IMPORTED)

set_target_properties(allegro_lib PROPERTIES
        IMPORTED_IMPLIB "C:/Windows/SysWOW64/alleg44.dll"
        IMPORTED_LOCATION "C:/Windows/SysWOW64/alleg44.dll"
)

add_executable(projet_algo_s2
        src/main.c)

target_link_libraries(projet_algo_s2 PRIVATE allegro_lib)