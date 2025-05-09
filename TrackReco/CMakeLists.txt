include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )
FILE( GLOB src_files *.cxx )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

#art_make_library(
#  SOURCE
#    SingleTrackAlgo.cxx
art_make_library ( SOURCE ${src_files}
        LIBRARY_NAME  SingleTrackAlgo
 		   LIBRARIES
                   RunHistory
    	           Geometry
                   Geometry_service
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
)

#art_make_library ( SOURCE ${src_files}
#                   LIBRARY_NAME  TrackReco
#                   LIBRARIES
#		   ToFUtil
#		   RawData
#		   RecoBase
#		   ChannelMap
#			RunHistory
#		   art_root_io::TFileService_service
#		   messagefacility::MF_MessageLogger
#		   ROOT::Core
#                   ROOT::Hist
#	           ROOT::Spectrum
#		   ROOT::Tree
#)

cet_build_plugin(MakeSingleTracks art::module
  LIBRARIES PRIVATE
    RawData
    RecoBase
    RecoUtils
    SingleTrackAlgo
    #SingleTrackAlgo.cxx
    ChannelMapService_service
    Geometry_service
    DetGeoMapService_service    
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Core
    ROOT::Hist
    ROOT::Spectrum
    ROOT::Tree
)
cet_build_plugin(MakeSingleTracksForAlign art::module
  LIBRARIES PRIVATE
    RawData
    RecoBase
    RecoUtils
    SingleTrackAlgo
    ChannelMapService_service
    Geometry_service
    DetGeoMapService_service
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Core
    ROOT::Hist
    ROOT::Spectrum
    ROOT::Tree
)
cet_build_plugin(MakeTrackSegments art::module
  LIBRARIES PRIVATE
    RawData
    RecoBase
    RecoUtils
    SingleTrackAlgo
    ChannelMapService_service
    Geometry_service
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
