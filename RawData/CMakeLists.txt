include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

cet_find_package(artdaq REQUIRED)

art_make_library(LIBRARY_NAME RawData
                 SOURCE ${src_files}
                 LIBRARIES PRIVATE
                           art::Framework_Core
                           art::Persistency_Common
                           canvas::canvas
                           fhiclcpp::fhiclcpp
			   artdaq_core::artdaq-core_Data
  )

#build_plugin(DAQHeaderAna "module"
#             LIBRARIES PRIVATE art_root_io::TFileService_service ROOT::Hist)

install_headers()
install_source()
install_fhicl()

#add_subdirectory(nova)

#art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::RawData)
