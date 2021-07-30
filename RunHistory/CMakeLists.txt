include(ArtDictionary)
include(ArtMake)

cet_find_library(LIBWDA NAMES wda PATHS ENV LIBWDA_LIB NO_DEFAULT_PATH)

include_directories( $ENV{LIBWDA_FQ_DIR}/include) 
include_directories( $ENV{ART_CPP_DB_INTERFACES_INC}) 

art_make_library(
  SOURCE
    RunHistory.cxx
  LIBRARIES PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ${LIBWDA}
  )

#build_plugin(DAQHeaderAna "module"
#             LIBRARIES PRIVATE art_root_io::TFileService_service ROOT::Hist)

install_headers()
install_source()
install_fhicl()

#add_subdirectory(nova)

#art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::RawData)
