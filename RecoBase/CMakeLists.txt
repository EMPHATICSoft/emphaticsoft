include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

art_make_library(
  SOURCE ${src_files}
  LIBRARIES PRIVATE
    RawData
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
  )

#build_plugin(DAQHeaderAna "module"
#             LIBRARIES PRIVATE art_root_io::TFileService_service ROOT::Hist)

install_headers()
install_source()
install_fhicl()

#add_subdirectory(nova)

#art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::RawData)
