include(ArtDictionary)
include(ArtMake)

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )

set( CET_DICT_FUNCTIONS TRUE)

FILE( GLOB src_files *.cpp )
art_make_library(
  SOURCE ${src_files}
)

install_headers()
install_source()
