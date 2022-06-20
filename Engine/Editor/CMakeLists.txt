cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project (MoltenEditor)

include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)

option(MOLTEN_ENABLE_COVERAGE "Enables code coverage reporting" OFF)

# Linux configurations.
if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
  find_package(Threads)
endif()

set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(CoreHeadersDir "${CoreDir}/Headers")
set(GraphicsDir "${EngineDir}/Graphics")
set(GraphicsHeadersDir "${GraphicsDir}/Headers")
set(EditorDir "${EngineDir}/Editor")
set(HeadersDir "${EditorDir}/Headers")
set(SourceDir "${EditorDir}/Source")
set(EditorHeadersDir "${HeadersDir}/Molten/Editor")
set(EditorSourceDir "${SourceDir}/Molten/Editor")
set(VendorDir "${EngineDir}/Vendor")

file(GLOB_RECURSE EditorHeaders "${HeadersDir}/*.h" "${HeadersDir}/*.hpp" "${HeadersDir}/*.inl")
file(GLOB_RECURSE EditorSources "${SourceDir}/*.c" "${SourceDir}/*.cpp")

# Add Core
if (NOT TARGET MoltenCore)
	add_subdirectory(${CoreDir} ${CoreDir} EXCLUDE_FROM_ALL)
endif()

# Add Graphics
if (NOT TARGET MoltenGraphics)
	add_subdirectory(${GraphicsDir} ${GraphicsDir} EXCLUDE_FROM_ALL)
endif() 

# Create Editor
add_executable(MoltenEditor "${EditorSources}" "${EditorHeaders}")
target_include_directories(MoltenEditor PUBLIC "${HeadersDir}")
target_include_directories(MoltenEditor PUBLIC "${CoreHeadersDir}")
target_include_directories(MoltenEditor PUBLIC "${GraphicsHeadersDir}")
target_include_directories(MoltenEditor PUBLIC "${VendorDir}")

# Configure defaults.
SetDefaultCompileOptions(MoltenEditor)
SetDefaultLinkerInputs(MoltenGraphics)
EnableMultiProcessorCompilation(MoltenEditor)
CreateSourceGroups("${EditorHeaders}" "${EditorHeadersDir}")
CreateSourceGroups("${EditorSources}" "${EditorSourceDir}")

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

target_link_libraries(MoltenEditor MoltenCore MoltenGraphics)