cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project (MoltenGraphics CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)

# Options.
option(MOLTEN_ENABLE_COVERAGE "Enables code coverage reporting" OFF)

# Get directories and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(CoreHeadersDir "${CoreDir}/Headers")
set(EditorFrameworkDir "${EngineDir}/EditorFramework")
set(EditorFrameworkHeadersDir "${EditorFrameworkDir}/Headers")
set(CliDir "${EngineDir}/Cli")
set(HeadersDir "${CliDir}/Headers")
set(SourceDir "${CliDir}/Source")
set(CliHeadersDir "${HeadersDir}/Molten/Cli")
set(CliSourceDir "${SourceDir}/Molten/Cli")
set(VendorDir "${EngineDir}/Vendor")

file(GLOB_RECURSE Headers "${HeadersDir}/*.h" "${HeadersDir}/*.hpp" "${HeadersDir}/*.inl")
file(GLOB_RECURSE Sources "${SourceDir}/*.c" "${SourceDir}/*.cpp")

# Linux configurations.
if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
  find_package(Threads)
endif()


# Add molten core
if (NOT TARGET MoltenCore)
	add_subdirectory(${CoreDir} ${CoreDir} EXCLUDE_FROM_ALL)
endif() 

if (NOT TARGET MoltenEditorFramework)
	add_subdirectory(${EditorFrameworkDir} ${EditorFrameworkDir} EXCLUDE_FROM_ALL)
endif() 

# Create library.
add_executable(MoltenCli "${Sources}" "${Headers}")
target_include_directories(MoltenCli PUBLIC "${HeadersDir}")
target_include_directories(MoltenCli PUBLIC "${CoreHeadersDir}")
target_include_directories(MoltenCli PUBLIC "${EditorFrameworkHeadersDir}")
target_include_directories(MoltenCli PUBLIC "${VendorDir}")

# Configure defaults.
SetDefaultCompileOptions(MoltenCli)
SetDefaultLinkerInputs(MoltenCli)
EnableMultiProcessorCompilation(MoltenCli)
CreateSourceGroups("${Headers}" "${CliHeadersDir}")
CreateSourceGroups("${Sources}" "${CliSourceDir}")

set_target_properties( MoltenCli
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenCliDebug"
  OUTPUT_NAME_RELEASE "MoltenCli"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Bin"
)

# Linking and setting compile definitions.
if(MOLTEN_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set_target_properties(MoltenCli PROPERTIES COMPILE_FLAGS "-lgcov --coverage")
  target_link_libraries(MoltenCli "gcov")
endif()

target_link_libraries(MoltenCli MoltenCore MoltenEditorFramework)
