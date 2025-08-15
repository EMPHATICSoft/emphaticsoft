cet_build_plugin(GetDataML art::module
  LIBRARIES PRIVATE
    RawData
    RecoBase
    DetGeoMap
    DetGeoMapService_service
    ChannelMap
    ChannelMapService_service
    GeometryService_service
    Geometry
    Simulation
    ARICHRecoUtils
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    ROOT::Core
    ROOT::Geom 
    ROOT::Tree
  BASENAME_ONLY
)

install_source()
install_fhicl()
install_headers()

