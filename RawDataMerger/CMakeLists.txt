include_directories($ENV{ARTDAQ_CORE_INC})
include_directories($ENV{CETLIB_EXCEPT_INC})
include_directories($ENV{TRACE_INC})

include(ArtDictionary)
include(ArtMake)

link_directories($ENV{ARTDAQ_CORE_LIB})
link_directories($ENV{CETLIB_EXCEPT_LIB})

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

cet_build_plugin(RawDataMerger art::module
  LIBRARIES #PRIVATE
    ${LIBRARY_CORE_LIB_LIST}
    emphatic-artdaq_Overlays
    RawData
    RawDataUnpacker
    # RecoBase
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    # ROOT::Hist
    artdaq-core_Data
)


install_headers()
install_fhicl()
install_source()
