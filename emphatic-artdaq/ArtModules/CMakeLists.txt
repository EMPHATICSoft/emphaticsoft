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
include(BuildPlugins)

# Yuck!
link_directories($ENV{ARTDAQ_CORE_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible
link_directories($ENV{ARTDAQ_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible
link_directories($ENV{ARTDAQ_UTILITIES_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible
link_directories($ENV{CETLIB_EXCEPT_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible

include_directories($ENV{ARTDAQ_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ARTDAQ_UTILITIES_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ARTDAQ_CORE_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{TRACE_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

  add_definitions(-DSIMPLER_PSET_MAKE) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_compile_definitions() or target_compile_features() whenever possible

cet_make_library(LIBRARY_NAME emphatic-artdaq_ArtModules
  SOURCE detail/TransferWrapper.cc
  LIBRARIES PUBLIC
  artdaq_DAQdata
  artdaq_TransferPlugins
  artdaq-utilities_Plugins
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
