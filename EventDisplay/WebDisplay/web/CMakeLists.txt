include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )
set( CET_DICT_FUNCTIONS TRUE)

FILE( GLOB src_files *.cpp )
cet_make_library(
  SOURCE ${src_files}
  LIBRARIES nlohmann_json::nlohmann_json
)

install_headers()
install_source()
