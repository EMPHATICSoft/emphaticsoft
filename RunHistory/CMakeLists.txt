include(ArtDictionary)
include(ArtMake)

find_package(libwda REQUIRED EXPORT)

art_make_library(SOURCE RunHistory.cxx
  LIBRARIES
  PRIVATE
  wda::wda
)

cet_make_exec( NAME emph_runHistory
  SOURCE emph_runHistory.cc
  LIBRARIES RunHistory
)

install_headers()
install_source()
install_fhicl()

add_subdirectory(service)
