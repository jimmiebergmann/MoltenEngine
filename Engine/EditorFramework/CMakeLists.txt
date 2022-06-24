cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project (MoltenEditorFramework CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)

# Options.
option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting." OFF)
option(MOLTEN_ENABLE_EDITORFRAMEWORK_TESTS "Enables editor framework tests." ON)

# Get directories and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(CoreHeadersDir "${CoreDir}/Headers")
set(EditorFrameworkDir "${EngineDir}/EditorFramework")
set(HeadersDir "${EditorFrameworkDir}/Headers")
set(SourceDir "${EditorFrameworkDir}/Source")
set(EditorFrameworkHeadersDir "${HeadersDir}/Molten/EditorFramework")
set(EditorFrameworkSourceDir "${SourceDir}/Molten/EditorFramework")
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

# Create library.
add_library(MoltenEditorFramework SHARED "${Sources}" "${Headers}")
target_include_directories(MoltenEditorFramework PUBLIC "${HeadersDir}")
target_include_directories(MoltenEditorFramework PUBLIC "${CoreHeadersDir}")
target_include_directories(MoltenEditorFramework PUBLIC "${VendorDir}")

# Configure defaults.
SetDefaultCompileOptions(MoltenEditorFramework)
SetDefaultLinkerInputs(MoltenEditorFramework)
EnableMultiProcessorCompilation(MoltenEditorFramework)
CreateSourceGroups("${Headers}" "${EditorFrameworkHeadersDir}")
CreateSourceGroups("${Sources}" "${EditorFrameworkSourceDir}")

# Set preprocessor definitions.
target_compile_definitions(MoltenEditorFramework PRIVATE MOLTEN_EDITOR_FRAMEWORK_EXPORT)
install(TARGETS MoltenEditorFramework DESTINATION "${RootDir}/Lib") # Install library
set_target_properties( MoltenEditorFramework
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenEditorFrameworkDebug"
  OUTPUT_NAME_RELEASE "MoltenEditorFramework"
  ARCHIVE_OUTPUT_DIRECTORY "${RootDir}/Lib"
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Lib"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Lib"
  LIBRARY_OUTPUT_DIRECTORY "${RootDir}/Lib"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Lib"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Lib"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Bin"
)

# Linking and setting compile definitions.
if(MOLTEN_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set_target_properties(MoltenEditorFramework PROPERTIES COMPILE_FLAGS "-lgcov --coverage")
  target_link_libraries(MoltenEditorFramework "gcov")
endif()

target_link_libraries(MoltenEditorFramework MoltenCore)


# Add tests.
if(MOLTEN_ENABLE_EDITORFRAMEWORK_TESTS)
  add_subdirectory(Test)
endif()