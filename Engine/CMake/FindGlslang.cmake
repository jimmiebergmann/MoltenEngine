find_path(Glslang_DIR NAMES "glslang/Public/ShaderLang.h" PATHS "${ThirdPartyDir}/glslang")

if(NOT("${Glslang_DIR}" STREQUAL "Glslang_DIR-NOTFOUND"))
  set(Glslang_FOUND TRUE)
  set(Glslang_LIBRARY "glslang")
  set(SPIRV_LIBRARY "SPIRV")
else()
  set(Glslang_FOUND FALSE)
endif()