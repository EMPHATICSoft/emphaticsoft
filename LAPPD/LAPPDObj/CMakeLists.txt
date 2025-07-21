FILE( GLOB src_files *.cxx )

find_package(artdaq_core REQUIRED)
find_package(messagefacility REQUIRED)

art_make_library(LIBRARY_NAME lappd_obj
                SOURCE ${src_files}
                LIBRARIES PRIVATE
                        art::Framework_Core
                        art::Persistency_Common
                        messagefacility::MF_MessageLogger
                        canvas::canvas
                        fhiclcpp::fhiclcpp
                        artdaq_core::artdaq-core_Data
)

install_headers()
install_source()
install_fhicl()

art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::lappd_obj)
