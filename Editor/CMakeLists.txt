project (CurseEditor)
cmake_minimum_required(VERSION 3.1)
include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)

option(CURSE_ENABLE_COVERAGE "Enables coverage reporting" OFF)

find_package(Threads)

# Set compiler flags
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -fno-elide-constructors")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(CoreDir "${RootDir}/Core")
set(CoreHeadersDir "${CoreDir}/Headers")
set(EditorDir "${CMAKE_CURRENT_SOURCE_DIR}")
set(EditorHeadersDir "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
set(EditorSourceDir "${CMAKE_CURRENT_SOURCE_DIR}/Source")
file(GLOB_RECURSE EditorHeaders "${EditorHeadersDir}/*.h" "${EditorHeadersDir}/*.hpp" "${EditorHeadersDir}/*.inl")
file(GLOB_RECURSE EditorSources "${EditorSourceDir}/*.c" "${EditorSourceDir}/*.cpp")

message("Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message("Code coverage: ${CURSE_ENABLE_COVERAGE}")

if (NOT TARGET Curse)
	add_subdirectory(${CoreDir} ${CoreDir} EXCLUDE_FROM_ALL)
endif() 

include_directories ("${EditorHeadersDir}")				 
include_directories ("${CoreHeadersDir}")

add_executable(CurseEditor "${EditorSources}" "${EditorHeaders}")

CreateSourceGroups("${EditorSources}" "${EditorSourceDir}")
CreateSourceGroups("${EditorHeaders}" "${EditorHeadersDir}")

set_target_properties( CurseEditor
  PROPERTIES
  OUTPUT_NAME_DEBUG "CurseEditorDebug"
  OUTPUT_NAME_RELEASE "CurseEditor"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Bin"
)

if(CURSE_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU") ## Code coverage
  set_target_properties(CurseEditor PROPERTIES COMPILE_FLAGS "--coverage")
  target_link_libraries(CurseEditor "gcov")
endif()

target_link_libraries(CurseEditor Curse)
