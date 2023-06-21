include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

art_make_library(SOURCE ${src_files}
                 LIBRARIES PRIVATE
                           RecoBase
                           art::Framework_Core
                           art::Persistency_Common
                           canvas::canvas
                           ROOT::Physics
)

cet_build_plugin(MakeTrack art::module
        LIBRARIES PRIVATE
        ChannelMap
        ChannelMapService_service
        RunHistory
        RunHistoryService_service
        RawData
        RecoBase
        Geometry_service
        Geometry
        ROOT::Tree
        art_root_io::TFileService_service
)

install_headers()
install_source()
install_fhicl()
