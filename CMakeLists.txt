project (CurseEngine)
cmake_minimum_required(VERSION 2.8)

option(CODE_COVERAGE "Enables coverage reporting" OFF)

add_subdirectory(core)
add_subdirectory(test)