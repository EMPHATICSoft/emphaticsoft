include(ArtDictionary)
include(ArtMake)

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

set( CET_DICT_FUNCTIONS TRUE)

FILE( GLOB src_files *.cpp )
art_make_library(
  SOURCE ${src_files}
)

install_headers()
install_source()
