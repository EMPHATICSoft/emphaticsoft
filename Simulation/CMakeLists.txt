include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

art_make_library(
  SOURCE ${src_files}
  LIBRARIES PRIVATE
  art::Framework_Core
  )

install_headers()
install_source()
install_fhicl()

