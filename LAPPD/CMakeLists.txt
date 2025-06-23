include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )

add_subdirectory(LAPPDObj)
add_subdirectory(LAPPDUnpacker)

cet_build_plugin(LAPPDEventGen art::source SOURCE 
  LIBRARIES   PRIVATE
    ROOT::Core
    ROOT::Tree
    ROOT::Hist
    artdaq_core::artdaq-core_Data
    artdaq_core::artdaq-core_Utilities
    art_root_io::TFileService_service
    art::Framework_Core
    art::Framework_IO_Sources
    art::Framework_Principal
    art::Framework_Services_Registry
    art_root_io::TFileService_service
    lappd_obj
    lappd_unpacker
)

cet_build_plugin(LAPPDWaveformDisplayAna art::module
  LIBRARIES PRIVATE
  art_root_io::TFileService_service
  messagefacility::MF_MessageLogger
  ROOT::Hist
  ROOT::Tree
  lappd_obj
)

add_subdirectory(config)
add_subdirectory(job)

install_headers()
install_source()
install_fhicl()
