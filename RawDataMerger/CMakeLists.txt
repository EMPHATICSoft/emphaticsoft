

cet_build_plugin(RawDataMerger art::module
  LIBRARIES REG
    artdaq_core::artdaq-core_Data
    TRACE::TRACE
    emphatic-artdaq_Overlays
    RawData
    RawDataUnpacker
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    cetlib_except::cetlib_except
 BASENAME_ONLY
)

install_fhicl()
install_source()
