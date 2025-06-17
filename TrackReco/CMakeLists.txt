cet_make_library ( SOURCE SingleTrackAlgo.cxx
  LIBRARY_NAME  SingleTrackAlgo
  LIBRARIES
    RunHistory
    Geometry
    GeometryService_service
    RecoBase
    RecoUtils
    DetGeoMapService_service
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
  BASENAME_ONLY
)

cet_build_plugin(MakeSingleTracks art::module
  LIBRARIES REG
    RawData
    RecoBase
    RecoUtils
    SingleTrackAlgo
    ChannelMapService_service
    GeometryService_service
    DetGeoMapService_service    
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Core
    ROOT::Hist
    ROOT::Spectrum
    ROOT::Tree
)

cet_build_plugin(MakeSingleTracksForAlign art::module
  LIBRARIES REG
    RawData
    RecoBase
    RecoUtils
    SingleTrackAlgo
    ChannelMapService_service
    GeometryService_service
    DetGeoMapService_service
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Core
    ROOT::Hist
    ROOT::Spectrum
    ROOT::Tree
)

cet_build_plugin(MakeTrackSegments art::module
  LIBRARIES REG
    RawData
    RecoBase
    RecoUtils
    SingleTrackAlgo
    ChannelMapService_service
    GeometryService_service
    DetGeoMapService_service
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Core
    ROOT::Hist
    ROOT::Spectrum
    ROOT::Tree
)

install_source()
install_fhicl()
install_headers()
