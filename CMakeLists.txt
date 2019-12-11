cmake_minimum_required(VERSION 3.15)
project(iseccrack C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

find_package(OpenMP)

add_executable(iseccrack gpu.c)

target_link_libraries(iseccrack m)
target_link_libraries(iseccrack OpenMP::OpenMP_C)
target_link_libraries(iseccrack gmp)