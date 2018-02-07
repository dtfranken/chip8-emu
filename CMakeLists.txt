cmake_minimum_required(VERSION 3.9)
project(chip8_emu)

set(CMAKE_CXX_STANDARD 17)

add_executable(chip8_emu main.cpp core.h)