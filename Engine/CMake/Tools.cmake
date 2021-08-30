# Function for grouping files. Used for filters in Visual Studio.
function(CreateSourceGroups files rootPath)
  foreach(_source IN ITEMS ${files} )
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${rootPath}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
  endforeach()
endfunction(CreateSourceGroups)

# Function for setting working directory of Visual Studio project.
function(SetVisualStudioWorkingDir proj workingDir)
  if((${CMAKE_VERSION} VERSION_EQ "3.8.0") OR (${CMAKE_VERSION} VERSION_GREATER "3.8.0")) 
    set_property(TARGET ${proj} APPEND PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${workingDir}")
  endif()	
endfunction(SetVisualStudioWorkingDir)

# Set default compiler options.
function(SetDefaultCompileOptions target)
  if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang|GNU")  
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
      target_compile_options(${target} PRIVATE -Wmove)
    endif()   
    target_compile_options(${target} PRIVATE -Werror -Wall -Wextra -Wno-long-long)    
    if("${CMAKE_BUILD_TYPE}" MATCHES Debug)
      target_compile_options(${target} PRIVATE -g)
    else()
      target_compile_options(${target} PRIVATE -O3)
    endif()

  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /W4 /WX /w44062)
  endif()

  get_target_property(compilerFlags ${target} COMPILE_OPTIONS)

  message("Setting \"target_compile_options\": \"${compilerFlags}\" for ${CMAKE_CXX_COMPILER_ID}, target ${target}.")

  # C++ standard version.
  set_target_properties(${target}
    PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED OFF
      CXX_EXTENSIONS OFF
  )

endfunction(SetDefaultCompileOptions)

# Add multi-processor compilation.
function(EnableMultiProcessorCompilation target)

  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /MP)
  endif()
  
  get_target_property(compilerFlags ${target} COMPILE_OPTIONS)
  message("Setting \"target_compile_options\": \"${compilerFlags}\" for ${CMAKE_CXX_COMPILER_ID}, target ${target}.")
  
endfunction(EnableMultiProcessorCompilation)
