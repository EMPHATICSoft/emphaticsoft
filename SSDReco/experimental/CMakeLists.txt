include(ArtDictionary)
include(ArtMake)
FILE(GLOB experimental_src_files *.cxx)

art_make_library(LIBRARY_NAME SSDRecoExperimentalUtil
  SOURCE ${experimental_src_files}
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
    Geometry_service
    Geometry
    Align
    AlignService_service
    MagneticField
    MagneticField_service
    ROOT::Core
    ROOT::MathCore
    ROOT::Minuit2
    ROOT::Physics
    art_root_io::TFileService_service
)

cet_build_plugin(StudyOneSSDClusters art::module
  LIBRARIES PRIVATE
    SSDRecoExperimentalUtil
    SSDRecoUtil
    art_root_io::TFileService_service
    Geometry_service
    MagneticField_service
    AlignService_service
    DetGeoMapService_service
)

cet_build_plugin(RecoBeamTracksAutre art::module
  LIBRARIES PRIVATE
    SSDRecoExperimentalUtil
    SSDRecoUtil
    art_root_io::TFileService_service
    Geometry_service
    MagneticField_service
    AlignService_service
    DetGeoMapService_service
)

cet_build_plugin(StudyAllTrial1Autre art::module
  LIBRARIES PRIVATE
    SSDRecoExperimentalUtil
    SSDRecoUtil
    art_root_io::TFileService_service
    Geometry_service
    MagneticField_service
    AlignService_service
    DetGeoMapService_service
)

install_headers()
install_source()
install_fhicl()
