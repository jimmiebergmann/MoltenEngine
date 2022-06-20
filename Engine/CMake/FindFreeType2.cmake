find_path(
  FreeType2_DIR
  NAMES "include/ft2build.h"
  PATHS "${RootDir}/Engine/Vendor/ThirdParty/FreeType2"
  NO_DEFAULT_PATH)

if(NOT("${FreeType2_DIR}" STREQUAL "FreeType2_DIR-NOTFOUND"))
  set(FreeType2_FOUND TRUE)
  set(FreeType2_INCLUDE "${FreeType2_DIR}/include")
  set(FreeType2_LIBRARY "freetype")
else()
  set(FreeType2_FOUND FALSE)
endif()