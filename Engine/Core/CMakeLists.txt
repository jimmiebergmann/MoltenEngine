cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project (Molten CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)

# Options.
option(MOLTEN_ENABLE_VULKAN "Enables Vulkan renderer." ON)
option(MOLTEN_ENABLE_OPENGL "Enables OpenGL renderer." ON)
option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting." OFF)
option(MOLTEN_ENABLE_X11 "Enables X server." ON)

# Main directories and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(VendorDir "${EngineDir}/Vendor")
set(ThirdPartyDir "${EngineDir}/Vendor/ThirdParty")
set(CoreHeadersDir "${CoreDir}/Headers")
set(CoreSourceDir "${CoreDir}/Source")
file(GLOB_RECURSE CoreHeaders "${CoreHeadersDir}/*.h" "${CoreHeadersDir}/*.hpp" "${CoreHeadersDir}/*.inl")
file(GLOB_RECURSE CoreSources "${CoreSourceDir}/*.c" "${CoreSourceDir}/*.cpp")

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
	add_subdirectory("${FreeType2_DIR}" "${FreeType2_DIR}")
else()
	message(SEND_ERROR "Cannot find FreeType2.")
endif()

include_directories ("${CoreHeadersDir}")
include_directories ("${ThirdPartyDir}")
add_library(Molten SHARED "${CoreSources}" "${CoreHeaders}")

SetDefaultCompileOptions(Molten)
EnableMultiProcessorCompilation(Molten)

CreateSourceGroups("${CoreSources}" "${CoreSourceDir}")
CreateSourceGroups("${CoreHeaders}" "${CoreHeadersDir}")

target_compile_definitions(Molten PRIVATE MOLTEN_EXPORT)
install(TARGETS Molten DESTINATION "${RootDir}/Lib") # Install library
set_target_properties( Molten
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenDebug"
  OUTPUT_NAME_RELEASE "Molten"
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
  set_target_properties(Molten PROPERTIES COMPILE_FLAGS "-lgcov --coverage")
  target_link_libraries(Molten "gcov")
endif()

if(MOLTEN_ENABLE_VULKAN AND Vulkan_FOUND)
  target_include_directories(Molten PUBLIC "${Vulkan_INCLUDE}")
  target_link_libraries(Molten "${Vulkan_LIBRARY}")
  target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_VULKAN")
endif()

if(MOLTEN_ENABLE_OPENGL AND OpenGL_FOUND)
  target_link_libraries(Molten "${OPENGL_LIBRARIES}")
  target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_OPENGL")
endif()

if(MOLTEN_ENABLE_X11)
  target_link_libraries(Molten "X11")
  target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_X11")
endif()

if(FreeType2_FOUND)
  target_include_directories(Molten PUBLIC "${FreeType2_INCLUDE}")
  target_link_libraries(Molten "${FreeType2_LIBRARY}")
  set_target_properties("${FreeType2_LIBRARY}" PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_include_directories(Molten PUBLIC "${VendorDir}")


# Print summary
message("MOLTEN SUMMARY: ")
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
