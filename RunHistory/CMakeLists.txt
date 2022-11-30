include(ArtDictionary)
include(ArtMake)

find_package(libwda REQUIRED EXPORT)

art_make_library(SOURCE RunHistory.cxx
  LIBRARIES
  PRIVATE
  wda::wda
)

target_include_directories(RunHistory BEFORE
  PRIVATE "$ENV{ART_CPP_DB_INTERFACES_INC}"
)

install_headers()
install_source()
install_fhicl()

add_subdirectory(service)
