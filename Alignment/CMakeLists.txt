include_directories( $ENV{ROOT_INC} )
include_directories( $ENV{ART_ROOT_IO_INC} )
include_directories( $ENV{MILLE_INC} )

link_directories( $ENV{MILLEPEDE_II_LIB} )
link_directories( $ENV{MILLE_LIB} )

cet_make_library(SOURCE MilleRecord.cxx MilleRun.cxx MilleResult.cxx GSLStudies.cxx
   LIBRARY_NAME AlignMilleRecords
   LIBRARIES
        art::Framework_Core
	ROOT::Core
	ROOT::Geom
	fhiclcpp::fhiclcpp
	gsl
	cblas
        BASENAME_ONLY
)

cet_build_plugin(SingleTrackAlignment art::module
  LIBRARIES REG RawData
        AlignMilleRecords
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
	ROOT::Tree
	mille
)
cet_build_plugin(SingleTrackAlignmentV2 art::module
  LIBRARIES REG RawData
        AlignMilleRecords
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
	ROOT::Tree
	mille
	gsl
)
cet_build_plugin(CheckAlignFrontMagnet art::module
  LIBRARIES REG RawData
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
	ROOT::Tree
)

install_source()
install_fhicl()
install_headers()
