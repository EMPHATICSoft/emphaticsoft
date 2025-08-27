art_dictionary(DICTIONARY_LIBRARIES PRIVATE ARICHRecoUtils)

FILE( GLOB srcs *.cc )

cet_make_library( SOURCE ${srcs}
		  LIBRARY_NAME ARICHRecoUtils
		  LIBRARIES 
		  Geometry
		  GeometryService_service
		  Simulation
		  ChannelMap
		  ChannelMapService_service
		  art::Framework_Core
		  art::Persistency_Common
		  messagefacility::MF_MessageLogger
		  ROOT::Core
		  ROOT::Physics
		  ROOT::Hist
)

install_headers()
install_source()
