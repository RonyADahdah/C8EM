cmake_minimum_required(VERSION 4.2)
project(C8EM C)

set(CMAKE_C_STANDARD 23)

find_package(SDL3 REQUIRED CONFIG)

add_executable(C8EM main.c
        cpu.c
        cpu.h
        emu.h
        frontend.c
        frontend.h)

target_link_libraries(C8EM SDL3::SDL3)
