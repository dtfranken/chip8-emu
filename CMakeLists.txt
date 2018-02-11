cmake_minimum_required(VERSION 3.9)
project(chip8_emu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lcygwin -lSDL2main -lSDL2 -mwindows")

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(chip8_emu main.cpp core.h core.cpp keyboard.cpp keyboard.h timer.cpp timer.h)

target_link_libraries(chip8_emu SDL2main SDL2)
