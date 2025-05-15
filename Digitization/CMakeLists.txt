include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

#art_make_library (LIBRARY_NAME  Digitization 
#  SOURCE ${src_files}
#  LIBRARIES     SimulationBase
#                Simulation
#		G4Base
#		Geometry
#		RawData
#		ChannelMap
#		ChannelMapService_service
#		 DetGeoMap
#   		DetGeoMapService_service
#		art_root_io::TFileService_service
#		art::Framework_Core
#	        art::Persistency_Common
#		messagefacility::MF_MessageLogger
#		)

cet_build_plugin(SSDDigitizer art::module
  LIBRARIES PRIVATE
	SimulationBase
        Simulation
        G4Base
        Geometry
        RawData
        ChannelMap
        ChannelMapService_service
        DetGeoMap
    	DetGeoMapService_service
	art_root_io::TFileService_service
        art::Framework_Core
        art::Persistency_Common
        messagefacility::MF_MessageLogger
)

cet_build_plugin(ARICHDigitizer art::module
  LIBRARIES PRIVATE
        SimulationBase
        Simulation
        G4Base
        Geometry
        RawData
        ChannelMap
        ChannelMapService_service
        DetGeoMap
        DetGeoMapService_service
        art_root_io::TFileService_service
        art::Framework_Core
        art::Persistency_Common
        messagefacility::MF_MessageLogger
)

cet_build_plugin(RelabelRawDigitModules art::module
  LIBRARIES PRIVATE
        SimulationBase
        Simulation
        G4Base
        Geometry
        RawData
        ChannelMap
        ChannelMapService_service
        DetGeoMap
        DetGeoMapService_service
        art_root_io::TFileService_service
        art::Framework_Core
	art::Framework_Principal
	art_root_io::tfile_support ROOT::Core        
	art::Framework_Services_Registry
        art::Persistency_Common
        messagefacility::MF_MessageLogger
	art::Framework_IO_ProductMix
)

install_source()
install_headers()
install_fhicl()
