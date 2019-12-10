cmake_minimum_required(VERSION 3.15)
project(iseccrack C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

add_executable(iseccrack main.c)

target_link_libraries(iseccrack m)

find_package(OpenMP)
target_link_libraries(iseccrack OpenMP::OpenMP_C)
target_link_libraries(iseccrack gmp)