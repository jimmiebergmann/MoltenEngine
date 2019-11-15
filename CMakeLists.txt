project (CurseEngine)
cmake_minimum_required(VERSION 3.1)

option(CODE_COVERAGE "Enables coverage reporting" OFF)

add_subdirectory(Core)
add_subdirectory(Editor)
add_subdirectory(Test)