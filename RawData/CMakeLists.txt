include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

<<<<<<< HEAD
art_make_library(LIBRARY_NAME RawData
                 SOURCE ${src_files}
                 LIBRARIES PRIVATE
                           art::Framework_Core
                           art::Persistency_Common
                           canvas::canvas
                           fhiclcpp::fhiclcpp
=======
art_make_library(
  SOURCE
    RawDigit.cxx
    SSDRawDigit.cxx
    LIBRARIES PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
>>>>>>> 3c00f714408bb9144ec5f6c528a08038fa947c8c
  )

#build_plugin(DAQHeaderAna "module"
#             LIBRARIES PRIVATE art_root_io::TFileService_service ROOT::Hist)

install_headers()
install_source()
install_fhicl()

#add_subdirectory(nova)

#art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::RawData)
