include(ArtDictionary)
include(ArtMake)

art_make_library(
  SOURCE
    Geometry.cxx
  LIBRARIES PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Geom
#    ROOT::GDML
#    root::XMLIO
#    root::EG
  )

#build_plugin(DAQHeaderAna "module"
#             LIBRARIES PRIVATE art_root_io::TFileService_service ROOT::Hist)

install_headers()
install_source()
install_fhicl()

#add_subdirectory(nova)

#art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::RawData)
