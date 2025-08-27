set (ART_build_LIB_LIST
  art::Framework_Core
  art::Framework_Principal
  art::Persistency_Provenance
  art::Utilities
  art_root_io::TFileService_service
)

set (ARTDAQ_build_LIB_LIST
  artdaq_core::artdaq-core_Data
  artdaq_core::artdaq-core_Utilities
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

add_definitions(-DSIMPLER_PSET_MAKE) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_compile_definitions() or target_compile_features() whenever possible

cet_make_library(LIBRARY_NAME emphatic-artdaq_ArtModules
  SOURCE detail/TransferWrapper.cc
  LIBRARIES PUBLIC
    artdaq::artdaq_DAQdata
    artdaq::artdaq_TransferPlugins
    artdaq::artdaq-utilities_Plugins
    ${ARTDAQ_build_LIB_LIST}
    art_root_io::art_root_io
    fhiclcpp::fhiclcpp
    messagefacility::MF_MessageLogger
  )

build_plugin(EmphaticOMInput "source" LIBRARIES REG
	emphatic-artdaq_ArtModules
) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - deprecated: use cet_build_plugin() with explicit plugin base types

build_plugin( TRB3Dump "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - deprecated: use cet_build_plugin() with explicit plugin base types

install_fhicl()
