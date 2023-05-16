cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project (MoltenCore CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)

# Options.
option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting." OFF)
option(MOLTEN_ENABLE_CORE_TESTS "Enables core tests." ON)

# Get directories and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(HeadersDir "${CoreDir}/Headers")
set(SourceDir "${CoreDir}/Source")
set(CoreMoltenHeadersDir "${HeadersDir}/Molten")
set(CoreMoltenSourceDir "${SourceDir}/Molten")
set(VendorDir "${EngineDir}/Vendor")

file(GLOB_RECURSE CoreHeaders "${HeadersDir}/*.h" "${HeadersDir}/*.hpp" "${HeadersDir}/*.inl")
file(GLOB_RECURSE CoreSources "${SourceDir}/*.c" "${SourceDir}/*.cpp")

# Linux configurations.
if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
  find_package(Threads)
endif()

# Create library.
add_library(MoltenCore SHARED "${CoreSources}" "${CoreHeaders}")
target_include_directories(MoltenCore PUBLIC "${HeadersDir}")
target_include_directories(MoltenCore PUBLIC "${VendorDir}")

# Configure defaults.
SetDefaultCompileOptions(MoltenCore)
SetDefaultLinkerInputs(MoltenCore)
EnableMultiProcessorCompilation(MoltenCore)
CreateSourceGroups("${CoreHeaders}" "${CoreMoltenHeadersDir}")
CreateSourceGroups("${CoreSources}" "${CoreMoltenSourceDir}")

# Set preprocessor definitions.
target_compile_definitions(MoltenCore PRIVATE MOLTEN_CORE_EXPORT)
install(TARGETS MoltenCore DESTINATION "${RootDir}/Lib") # Install library
set_target_properties( MoltenCore
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenCoreDebug"
  OUTPUT_NAME_RELEASE "MoltenCore"
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
  set_target_properties(MoltenCore PROPERTIES COMPILE_FLAGS "-lgcov --coverage")
  target_link_libraries(MoltenCore "gcov")
endif()


# Add tests.
if(MOLTEN_ENABLE_CORE_TESTS)
  add_subdirectory(Test)
endif()
