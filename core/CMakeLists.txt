project (Curse)
cmake_minimum_required(VERSION 2.8)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Tools.cmake)

find_package(Threads)

option(CODE_COVERAGE "Enables coverage reporting" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CoreIncludeDir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(CoreSourceDir "${CMAKE_CURRENT_SOURCE_DIR}/source")
file(GLOB_RECURSE CoreHeaders "${CoreIncludeDir}/*.h" "${CoreIncludeDir}/*.hpp" "${CoreIncludeDir}/*.inl")
file(GLOB_RECURSE CoreSources "${CoreSourceDir}/*.c" "${CoreSourceDir}/*.cpp")

message("${CMAKE_CURRENT_SOURCE_DIR}")
message("${CoreIncludeDir}")
message("${CoreSourceDir}")
message("${CoreHeaders}")
message("${CoreSources}")


include_directories ("${CoreIncludeDir}")
add_library(Curse SHARED ${CoreSources} ${CoreHeaders})  

CreateSourceGroups("${CoreSources}" "${CoreSourceDir}")
CreateSourceGroups("${CoreHeaders}" "${CoreIncludeDir}")

target_compile_definitions(Curse PRIVATE CURSE_EXPORT)
install(TARGETS Curse DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/lib") # Install library
set_target_properties( Curse
  PROPERTIES
  OUTPUT_NAME_DEBUG "CurseDebug"
  OUTPUT_NAME_RELEASE "Curse"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
)


