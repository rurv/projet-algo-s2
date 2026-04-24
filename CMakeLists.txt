cmake_minimum_required(VERSION 3.10)
project(projet_algo_s2 C)

set(CMAKE_C_STANDARD 11)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

add_executable(projet_algo_s2
        src/main.c
        src/ihm.c
        headers/ihm.h
        src/display.c
        headers/display.h
        src/game.c
        headers/game.h
        src/audio.c
        headers/audio.h
        src/assets.c
        headers/assets.h
        src/player.c
        headers/player.h
        src/asteroids.c
        headers/asteroids.h
        src/config.c
        headers/config.h
        src/save.c
        headers/save.h
        src/boss.c
        headers/boss.h)

target_link_libraries(projet_algo_s2 -lmingw32 -lalleg44)
