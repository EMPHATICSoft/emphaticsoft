# Using/not using art processing framework
if(DEFINED ENV{ART_VERSION})
    message(status " Building within art framework" )
    add_definitions(-DLAPPDFRAME_ART=1)
else()
    message(status " Building outside art framework" )
    add_definitions(-DLAPPDFRAME_ART=0)
endif()


FILE( GLOB src_files *.cc )

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

cet_build_plugin(LAPPDSignalProcessing art::module
  LIBRARIES PRIVATE
  art_root_io::TFileService_service
  messagefacility::MF_MessageLogger
  lappd_obj
  lappd_reco
)

cet_build_plugin(LAPPDRawMerger art::module
  LIBRARIES PRIVATE
  art_root_io::TFileService_service
  messagefacility::MF_MessageLogger
  lappd_obj
  lappd_unpacker
)

add_subdirectory(config)
add_subdirectory(job)

install_headers()
install_source()