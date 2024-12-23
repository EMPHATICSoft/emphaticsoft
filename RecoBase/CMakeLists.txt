include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

find_package(ROOT REQUIRED)

art_make_library(
  SOURCE ${src_files}
  LIBRARIES #PRIVATE
    RawData
    Geometry
    ROOT::Physics
    ROOT::Geom
    StandardRecord
  )

install_headers()
install_source()
install_fhicl()

art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::RecoBase)
