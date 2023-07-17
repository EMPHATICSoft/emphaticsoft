find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )

art_make_library(
  LIBRARY_NAME field_map_tools
  SOURCE
    inputtextfile.cpp
    quadd.cpp
    polyMint.cpp
  LIBRARIES
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
    )

install_headers()
install_source()
