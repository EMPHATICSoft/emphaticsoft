include_directories($ENV{ARTDAQ_CORE_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{CETLIB_EXCEPT_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{TRACE_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

include(ArtDictionary)
include(ArtMake)

link_directories($ENV{ARTDAQ_CORE_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible
link_directories($ENV{CETLIB_EXCEPT_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

cet_build_plugin(RawDataMerger art::module
  LIBRARIES REG
    ${LIBRARY_CORE_LIB_LIST}
    emphatic-artdaq_Overlays
    RawData
    RawDataUnpacker
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    artdaq-core_Data
)


install_headers()
install_fhicl()
install_source()
