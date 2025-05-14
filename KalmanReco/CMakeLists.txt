include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )
FILE( GLOB src_files *.cxx )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library ( SOURCE ${src_files}
        LIBRARY_NAME  KalmanReco
 		   LIBRARIES
		   Geometry
		   MagneticField
                   RecoBase
		   messagefacility::MF_MessageLogger
                   fhiclcpp::fhiclcpp
                   ROOT::Core
                   ROOT::Geom
                   ROOT::Physics
		   ROOT::Matrix
)

cet_build_plugin(SingleTrackKalmanReco art::module
  LIBRARIES PRIVATE
  RecoBase
  KalmanReco
  Geometry_service
  MagneticField_service    
  messagefacility::MF_MessageLogger
  ROOT::Core
  ROOT::Geom
  ROOT::Physics
  ROOT::Matrix
  ROOT::Hist
  ROOT::Tree
)

install_source()
install_fhicl()
install_headers()
