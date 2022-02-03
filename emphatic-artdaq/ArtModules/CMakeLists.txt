set (ART_build_LIB_LIST
  art::Framework_Core
  art::Framework_Principal
  art::Persistency_Provenance
  art::Utilities
  art_root_io::TFileService_service
)

set (ARTDAQ_build_LIB_LIST
  artdaq-core_Data
  artdaq-core_Utilities
)

set(ROOT_build_LIB_LIST
  ROOT::Tree
  ROOT::TreePlayer
  ROOT::Hist
)

set(BUILD_PLUGIN_CORE_LIB_LIST
  ${ART_build_LIB_LIST}
  ${ARTDAQ_build_LIB_LIST}
  ${ROOT_build_LIB_LIST}
  emphatic-artdaq_Overlays
  RawData
)

include(ArtDictionary)
include(ArtMake)

link_directories($ENV{ARTDAQ_CORE_LIB})
link_directories($ENV{CETLIB_EXCEPT_LIB})

include_directories(artdaq_DAQdata)
include_directories($ENV{ARTDAQ_CORE_INC})
include_directories($ENV{TRACE_INC})

build_plugin( TRB3Dump "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )

install_fhicl()
