project (Molten)
cmake_minimum_required(VERSION 3.1)
include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)

# Options.
option(MOLTEN_ENABLE_VULKAN "Enables Vulkan renderer." ON)
option(MOLTEN_ENABLE_GLSLANG "Enables glslang compiler." ON)
option(MOLTEN_ENABLE_OPENGL "Enables OpenGL renderer." ON)
option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting." OFF)


# Linux configurations.
if(UNIX AND NOT APPLE)
	option(MOLTEN_ENABLE_X11 "Enables X server." ON)

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
	message("FOUND Vulkan: ${Vulkan_FOUND}, Lib: ${Vulkan_LIBRARY}")
	
	if(MOLTEN_ENABLE_GLSLANG)
		set(GlslangDir "$ENV{VULKAN_SDK}/glslang")
		if(EXISTS ${GlslangDir})
			option(BUILD_EXTERNAL "" OFF)
			add_subdirectory(${GlslangDir} ${GlslangDir})
			set(glslang_FOUND "true")
		endif()
	endif()
endif()

# OpenGL configurations
if(MOLTEN_ENABLE_OPENGL)
	find_package(OpenGL)
	message("FOUND OPENGL: ${OpenGL_FOUND}, Lib: ${OPENGL_LIBRARIES}")
endif()

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

# Directories and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(ThirdPartyDir "${EngineDir}/ThirdParty")
set(CoreHeadersDir "${CoreDir}/Headers")
set(CoreSourceDir "${CoreDir}/Source")
file(GLOB_RECURSE CoreHeaders "${CoreHeadersDir}/*.h" "${CoreHeadersDir}/*.hpp" "${CoreHeadersDir}/*.inl")
file(GLOB_RECURSE CoreSources "${CoreSourceDir}/*.c" "${CoreSourceDir}/*.cpp")

include_directories ("${CoreHeadersDir}")
include_directories ("${ThirdPartyDir}")
add_library(Molten SHARED "${CoreSources}" "${CoreHeaders}")  

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

if(MOLTEN_ENABLE_VULKAN AND Vulkan_FOUND )
  target_include_directories(Molten PUBLIC "${Vulkan_INCLUDE}")
  target_link_libraries(Molten ${Vulkan_LIBRARY})  
  target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_VULKAN")
endif()

if(MOLTEN_ENABLE_GLSLANG AND glslang_FOUND)
	target_include_directories(Molten PUBLIC ${GlslangDir})
	target_link_libraries(Molten glslang SPIRV)
	target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_GLSLANG")
endif()

if(MOLTEN_ENABLE_OPENGL AND OpenGL_FOUND )
	target_link_libraries(Molten ${OPENGL_LIBRARIES})
	target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_OPENGL")
endif()

if(MOLTEN_ENABLE_X11)
  target_link_libraries(Molten "X11")
	target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_X11")
endif()
