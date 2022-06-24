cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project (MoltenGraphics CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)

# Options.
option(MOLTEN_ENABLE_VULKAN "Enables Vulkan renderer." ON)
option(MOLTEN_ENABLE_OPENGL "Enables OpenGL renderer." ON)
option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting." OFF)
option(MOLTEN_ENABLE_X11 "Enables X server." ON)
option(MOLTEN_ENABLE_GRAPHICS_TESTS "Enables graphics tests." ON)

# Get directories and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(CoreHeadersDir "${CoreDir}/Headers")
set(GraphicsDir "${EngineDir}/Graphics")
set(HeadersDir "${GraphicsDir}/Headers")
set(SourceDir "${GraphicsDir}/Source")
set(GraphicsHeadersDir "${HeadersDir}/Molten/Graphics")
set(GraphicsSourceDir "${SourceDir}/Molten/Graphics")
set(VendorDir "${EngineDir}/Vendor")

file(GLOB_RECURSE Headers "${HeadersDir}/*.h" "${HeadersDir}/*.hpp" "${HeadersDir}/*.inl")
file(GLOB_RECURSE Sources "${SourceDir}/*.c" "${SourceDir}/*.cpp")

# Linux configurations.
if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
  find_package(Threads)
  
  if(MOLTEN_ENABLE_OPENGL AND (NOT MOLTEN_ENABLE_X11))
    message(FATAL_ERROR "X11(MOLTEN_ENABLE_X11) is required if OpenGL(MOLTEN_ENABLE_OPENGL) is enabled.")
  endif()
else()
  set(MOLTEN_ENABLE_X11 OFF)
endif()

# Vulkan configurations
if(MOLTEN_ENABLE_VULKAN)
  include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/FindVulkan.cmake)  
endif()

# OpenGL configurations
if(MOLTEN_ENABLE_OPENGL)
  find_package(OpenGL)
endif()

# FreeType2 configurations
include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/FindFreeType2.cmake)
if(FreeType2_FOUND)
  if(NOT TARGET "${FreeType2_LIBRARY}")
    add_subdirectory("${FreeType2_DIR}" "${FreeType2_DIR}")
    set_target_properties("${FreeType2_LIBRARY}" PROPERTIES POSITION_INDEPENDENT_CODE ON)
    EnableMultiProcessorCompilation("${FreeType2_LIBRARY}")
  endif()
else()
  message(SEND_ERROR "Cannot find FreeType2.")
endif()

# Add molten core
if (NOT TARGET MoltenCore)
	add_subdirectory(${CoreDir} ${CoreDir} EXCLUDE_FROM_ALL)
endif() 

# Create library.
add_library(MoltenGraphics SHARED "${Sources}" "${Headers}")
target_include_directories(MoltenGraphics PUBLIC "${HeadersDir}")
target_include_directories(MoltenGraphics PUBLIC "${CoreHeadersDir}")
target_include_directories(MoltenGraphics PUBLIC "${VendorDir}")

# Configure defaults.
SetDefaultCompileOptions(MoltenGraphics)
SetDefaultLinkerInputs(MoltenGraphics)
EnableMultiProcessorCompilation(MoltenGraphics)
CreateSourceGroups("${Headers}" "${GraphicsHeadersDir}")
CreateSourceGroups("${Sources}" "${GraphicsSourceDir}")

# Set preprocessor definitions.
target_compile_definitions(MoltenGraphics PRIVATE MOLTEN_GRAPHICS_EXPORT)
install(TARGETS MoltenGraphics DESTINATION "${RootDir}/Lib") # Install library
set_target_properties( MoltenGraphics
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenGraphicsDebug"
  OUTPUT_NAME_RELEASE "MoltenGraphics"
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
  set_target_properties(MoltenGraphics PROPERTIES COMPILE_FLAGS "-lgcov --coverage")
  target_link_libraries(MoltenGraphics "gcov")
endif()

target_link_libraries(MoltenGraphics MoltenCore)

if(MOLTEN_ENABLE_VULKAN AND Vulkan_FOUND)
  target_include_directories(MoltenGraphics PUBLIC "${Vulkan_INCLUDE}")
  target_link_libraries(MoltenGraphics "${Vulkan_LIBRARY}")
  target_compile_definitions(MoltenGraphics PUBLIC "MOLTEN_ENABLE_VULKAN")
endif()

if(MOLTEN_ENABLE_OPENGL AND OpenGL_FOUND)
  target_link_libraries(MoltenGraphics "${OPENGL_LIBRARIES}")
  target_compile_definitions(MoltenGraphics PUBLIC "MOLTEN_ENABLE_OPENGL")
endif()

if(MOLTEN_ENABLE_X11)
  target_link_libraries(MoltenGraphics "X11")
  target_compile_definitions(MoltenGraphics PUBLIC "MOLTEN_ENABLE_X11")
endif()

if(FreeType2_FOUND)
  target_include_directories(MoltenGraphics PUBLIC "${FreeType2_INCLUDE}")
  target_link_libraries(MoltenGraphics "${FreeType2_LIBRARY}")
endif()


# Add tests.
if(MOLTEN_ENABLE_GRAPHICS_TESTS)
  add_subdirectory(Test)
endif()


# Print summary
message("Molten Graphics summary: ")
message("Vulkan_FOUND:  ${Vulkan_FOUND}  ")
message("OpenGL_FOUND:  ${OpenGL_FOUND}")
message("FreeType2_FOUND:  ${FreeType2_FOUND}")

if(Vulkan_FOUND)
  message("Vulkan_INCLUDE: ${Vulkan_INCLUDE}")
  message("Vulkan_LIBRARY: ${Vulkan_LIBRARY}")
endif()

if(OpenGL_FOUND)
  message("OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
endif()

if(FreeType2_FOUND)
  message("FreeType2_INCLUDE: ${FreeType2_INCLUDE}")
  message("FreeType2_LIBRARY: ${FreeType2_LIBRARY}")
endif()
