FILE(GLOB experimental_src_files *.cxx)

cet_make_library(SOURCE ${experimental_src_files}
  LIBRARY_NAME SSDRecoExperimentalUtil
  LIBRARIES PRIVATE
    SSDRecoUtil
    ChannelMap
    ChannelMapService_service
    DetGeoMap
    DetGeoMapService_service
    RunHistory
    RunHistoryService_service
  RawData
  RecoBase
    Geometry
    GeometryService_service
    Align
    AlignService_service
   MagneticField
  MagneticFieldService_service
   ROOT::Core
    ROOT::MathCore
    ROOT::Minuit2
    ROOT::Physics
    art_root_io::TFileService_service
  BASENAME_ONLY
)

cet_build_plugin(StudyOneSSDClusters art::module
  LIBRARIES REG
    SSDRecoExperimentalUtil
    SSDRecoUtil
    GeometryService_service
    MagneticFieldService_service
    AlignService_service
    DetGeoMapService_service
    art_root_io::TFileService_service
  BASENAME_ONLY
)

cet_build_plugin(RecoBeamTracksAutre art::module
  LIBRARIES REG
    SSDRecoExperimentalUtil
    SSDRecoUtil
    GeometryService_service
    MagneticFieldService_service
    AlignService_service
    DetGeoMapService_service
    art_root_io::TFileService_service
  BASENAME_ONLY
)

cet_build_plugin(StudyAllTrial1Autre art::module
  LIBRARIES REG
    SSDRecoExperimentalUtil
    SSDRecoUtil
    GeometryService_service
    MagneticFieldService_service
    AlignService_service
    DetGeoMapService_service
    art_root_io::TFileService_service
  BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()
