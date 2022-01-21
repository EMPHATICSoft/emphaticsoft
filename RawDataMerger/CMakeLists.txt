include(ArtDictionary)
include(ArtMake)

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)

build_plugin(RawDataMerger "module"
  LIBRARIES PRIVATE
    RawData
    # RecoBase
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    # ROOT::Hist
)


install_headers()
install_fhicl()
install_source()
