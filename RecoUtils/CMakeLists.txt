include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

art_make_library( #LIBRARY_NAME RecoUtils
  SOURCE ${src_files}
  LIBRARIES PRIVATE
    RecoBase
    Geometry
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
  )

install_headers()
install_source()
install_fhicl()

#art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::RecoUtils)
