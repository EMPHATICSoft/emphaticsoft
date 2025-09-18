# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
art_dictionary(DICTIONARY_LIBRARIES PRIVATE RawData)

FILE( GLOB src_files *.cxx )

cet_make_library(SOURCE ${src_files}
  LIBRARY_NAME 
    RawData
  LIBRARIES PRIVATE
    art::Framework_Core
    art::Persistency_Common
    messagefacility::MF_MessageLogger
    canvas::canvas
    fhiclcpp::fhiclcpp
    artdaq_core::artdaq-core_Data
  BASENAME_ONLY
)

install_headers()
install_source()
