set (ART_build_LIB_LIST
  art::Framework_Core
  art::Framework_Principal
  art::Persistency_Provenance
  art::Utilities
  #canvas
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
)

set(BUILD_PLUGIN_FRAGNAMEHELPER_LIB_LIST
  emphatic-artdaq_Overlays
  MF_MessageLogger
  fhiclcpp
  cetlib_except
)

set(BUILD_PLUGIN_BUILDINFO_LIB_LIST
  emphatic-artdaq_BuildInfo
  MF_MessageLogger
  fhiclcpp
  cetlib
  cetlib_except
)

include(ArtDictionary)
include(ArtMake)

cet_make(
 NO_PLUGINS # WAnt different link lists for modues
 BASENAME_ONLY
 LIBRARY_NAME
   emphatic-artdaq_ArtModules
 LIB_LIBRARIES
   emphatic-artdaq_Overlays

)

include(ArtDictionary)
include(ArtMake)

link_directories($ENV{ARTDAQ_CORE_LIB})
link_directories($ENV{CETLIB_EXCEPT_LIB})

include_directories(artdaq_DAQdata)
include_directories($ENV{ARTDAQ_CORE_INC})
include_directories($ENV{TRACE_INC})

build_plugin( CAENV1730Dump "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )
build_plugin( CAENV1730WaveformAna "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )
#build_plugin( TRB3Dump "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )

install_fhicl()
