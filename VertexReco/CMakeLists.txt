cet_make_library ( SOURCE PrimaryVertexAlgo.cxx
  LIBRARY_NAME  PrimaryVertexAlgo
  LIBRARIES
    RecoBase
    RecoUtils
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
  BASENAME_ONLY
)

cet_build_plugin(MakePrimaryVertex art::module
  LIBRARIES REG
    RecoBase
    RecoUtils
    PrimaryVertexAlgo
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
