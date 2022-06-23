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

cet_find_package(TRACE REQUIRED)

include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# Yuck!
link_directories($ENV{ARTDAQ_CORE_LIB})
link_directories($ENV{ARTDAQ_LIB})
link_directories($ENV{ARTDAQ_UTILITIES_LIB})
link_directories($ENV{CETLIB_EXCEPT_LIB})

include_directories($ENV{ARTDAQ_INC})
include_directories($ENV{ARTDAQ_UTILITIES_INC})
include_directories($ENV{ARTDAQ_CORE_INC})
include_directories($ENV{TRACE_INC})

  add_definitions(-DSIMPLER_PSET_MAKE)

cet_make_library(LIBRARY_NAME emphatic-artdaq_ArtModules
  SOURCE detail/TransferWrapper.cc
  LIBRARIES PUBLIC
  artdaq_DAQdata
  artdaq-utilities_Plugins
  ${ARTDAQ_build_LIB_LIST}
  art_root_io::art_root_io
	fhiclcpp::fhiclcpp
	messagefacility::MF_MessageLogger
  )

build_plugin(EmphaticOMInput "source" LIBRARIES REG
	emphatic-artdaq_ArtModules
)

build_plugin( TRB3Dump "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )

install_fhicl()
