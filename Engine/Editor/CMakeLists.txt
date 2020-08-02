project (MoltenEditor)
cmake_minimum_required(VERSION 3.1)
include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)

option(MOLTEN_ENABLE_COVERAGE "Enables code coverage reporting" OFF)

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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(CoreHeadersDir "${CoreDir}/Headers")
set(EditorDir "${CMAKE_CURRENT_SOURCE_DIR}")
set(EditorHeadersDir "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
set(EditorSourceDir "${CMAKE_CURRENT_SOURCE_DIR}/Source")
file(GLOB_RECURSE EditorHeaders "${EditorHeadersDir}/*.h" "${EditorHeadersDir}/*.hpp" "${EditorHeadersDir}/*.inl")
file(GLOB_RECURSE EditorSources "${EditorSourceDir}/*.c" "${EditorSourceDir}/*.cpp")

if (NOT TARGET Molten)
	add_subdirectory(${CoreDir} ${CoreDir} EXCLUDE_FROM_ALL)
endif() 

include_directories ("${EditorHeadersDir}")				 
include_directories ("${CoreHeadersDir}")

add_executable(MoltenEditor "${EditorSources}" "${EditorHeaders}")

CreateSourceGroups("${EditorSources}" "${EditorSourceDir}")
CreateSourceGroups("${EditorHeaders}" "${EditorHeadersDir}")

set_target_properties( MoltenEditor
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenEditorDebug"
  OUTPUT_NAME_RELEASE "MoltenEditor"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Bin"
)

SetVisualStudioWorkingDir("MoltenEditor" "${RootDir}/Bin")

if(MOLTEN_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU") ## Code coverage
  set_target_properties(MoltenEditor PROPERTIES COMPILE_FLAGS "--coverage")
  target_link_libraries(MoltenEditor "gcov")
endif()

target_link_libraries(MoltenEditor Molten)
