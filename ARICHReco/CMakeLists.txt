cet_build_plugin(ARICHReco art::module
  LIBRARIES REG
    RawData
    StandardRecord
    RecoBase
    DetGeoMap
    DetGeoMapService_service
    ChannelMap
    ChannelMapService_service
    GeometryService_service
    Geometry
    RunHistory
    ARICHRecoUtils
    Simulation
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    ROOT::Core
    ROOT::Geom 
    ROOT::Tree
)


cet_build_plugin(MakeArichCluster art::module
  LIBRARIES REG
    RawData
    StandardRecord
    RecoBase
    DetGeoMap
    DetGeoMapService_service
    ChannelMap
    ChannelMapService_service
    GeometryService_service
    Geometry
    RunHistory
    Simulation
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    ROOT::Core
    ROOT::Geom 
    ROOT::Tree
)

cet_build_plugin( MakeRing art::module
  LIBRARIES REG
    RawData
    StandardRecord
    RecoBase
    DetGeoMap
    DetGeoMapService_service
    ChannelMap
    ChannelMapService_service
    GeometryService_service
    Geometry
    RunHistory
    Simulation
    ARICHRecoUtils
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    ROOT::Core
    ROOT::Geom 
    ROOT::Tree
)





install_source()
install_fhicl()
install_headers()
